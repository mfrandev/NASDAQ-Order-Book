#ifndef SHARD_MANAGER_H_
#define SHARD_MANAGER_H_

#include <utility>
#include <array>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstdint>

#include <ShardConsumer.hpp>
#include <VWAPQueryResults.hpp>
#include <TWAPQueryResults.hpp>
#include <BinaryMessageWrapper.h>
#include <PerStockOrderState.h>
#include <PerStockVWAPConstants.h>
#include <PerStockTWAPConstants.h>

#include <message_utils.hpp>
#include <string_utils.hpp>
#include <time_utils.hpp>

#include <ProcessSystemEvent.hpp>

#include <cliargs.h>

template<uint8_t numberOfShards, uint32_t shardSize>
class ShardManager {

    public:
    ShardManager() : 
        _queues(make_queues(std::make_index_sequence<numberOfShards>{})), 
        coreIndex(ThreadConstants::FIRST_CONSUMER_CORE)
        {
            _locates.reserve(16'000UL);
        }

    void setCLIArgs(CLIArgs args) noexcept { _args = args; }

    void start() {
        for(auto i = 0; i < numberOfShards; ++i) {
            // TODO: Check once the shard threads become CPU bound, and distribute the workload across cores 4-14 inclusive (pass coreIndex by value)
            _threads[i] = std::thread(
                &ShardConsumer::run, 
                &_shards[i], 
                std::ref(_queues[i]), 
                coreIndex, 
                std::ref(_locateIndexedPerStockState),
                std::ref(_symbols),
                std::ref(_locates),
                std::ref(_systemEventSequenceTracker)
            );
            coreIndex += 2;
        }
    }

    bool dispatch(BinaryMessageWrapper&& msg) {

        if(_args.metrics.includeVWAP)
            maybeOutputQueryForLastNMinutesVWAPForEachStock(msg.header.timestamp, _args.intervals.intervalVWAP);
        if(_args.metrics.includeTWAP)
            maybeOutputQueryForLastNMinutesTWAPForEachStock(msg.header.timestamp, _args.intervals.intervalTWAP);
        if(_args.metrics.includeRV)
            maybeOutputQueryForLastNMinutesRVForEachStock(msg.header.timestamp, _args.intervals.intervalRV);

        if(isIrrelevantMessageType(msg.header.messageType)) 
            return false;

        // ==================== System event handling must be apply across all shards ====================
        if(msg.header.messageType == MessageTypes::MESSAGE_TYPE_SYSTEM_EVENT) {
            // Move is safe since msg is never used again in this function
            processSystemEvent(std::move(msg), std::ref(_systemEventSequenceTracker));
            return true;
        }

        // ==================== For shard-handled messages ====================
        uint8_t queueIndex = getShardIndex(msg.header.stockLocate);
        while(_queues[queueIndex].buffer.size() == _queues[queueIndex].buffer.capacity()) {
            std::this_thread::yield();
        }
        _queues[queueIndex].buffer.push(msg);
        return true;
    }

    void dumpOrderStates(std::ostream& os) const {
        for (size_t i = 0; i < _locateIndexedPerStockState.size(); ++i) {
            const auto& orderStatePtr = _locateIndexedPerStockState[i].orderState;
            const auto& ledgerPtr = _locateIndexedPerStockState[i].ledger;
            if (!orderStatePtr && !ledgerPtr) {
                continue;
            }
            os << "===== Stock Locate " << i << " =====\n";
            if(orderStatePtr)
                orderStatePtr->dump(os);
            if(ledgerPtr)
                ledgerPtr->dump(os);
        }
    }

    void shutDownConsumers() {

        for(auto& shard: _shards) {
            shard.finished.store(true);
        }
        for(auto& thread: _threads) {
            thread.join();
        }
        for(auto& perStockState: _locateIndexedPerStockState) {
            perStockState.vwapPrefixes.flush(perStockState.vwap);
        }
        // std::cout << "Queue processed " << numMessages << " messages\n";
    }

    // ======================== VWAP Output Helper Function ========================
    void maybeOutputQueryForLastNMinutesVWAPForEachStock(uint64_t now, const uint64_t NMINUTES) {
        if(!shouldEmitVWAPSnapshot(now, NMINUTES)) return;
        const int FAIL = -1;
        const uint64_t INTERVAL = static_cast<uint64_t>(NMINUTES) * VWAPConstants::NANOSECOND_PER_MINUTE;

        std::cout << "Outputting last " << NMINUTES << " minutes vwap at " << toTimeString(now, ':') << std::endl;

        std::filesystem::path outputDir{"../../LiveOutput/vwap"};
        std::error_code ec;
        std::filesystem::create_directories(outputDir, ec);
        if(ec) {
            std::cerr << "Failed to ensure VWAP output directory exists: " << ec.message() << '\n';
            return;
        }

        auto filename = std::string("vwap_") + toTimeString(now, '_') + ".csv";
        std::ofstream output(outputDir / filename, std::ios::out | std::ios::trunc);
        if(!output.is_open()) {
            std::cerr << "Failed to open VWAP output file: " << filename << '\n';
            return;
        }

        output << "ticker,vwap,start,end\n";
        for(size_t locate = 0; locate < _locateIndexedPerStockState.size(); ++locate) {
            PerStockState& state = _locateIndexedPerStockState[locate];
            if(state.vwap.notional == 0) continue; // 0 notional implies no trading volume
            state.vwapPrefixes.maybeAdvanceVWAPBucket(now, state.vwap);
            auto queryResults = state.queryLastNMinutesVWAP(now, NMINUTES);
            double vwap = queryResults == std::nullopt ? FAIL : vwapAsDouble(queryResults.value());

            uint64_t intervalStart = queryResults ? queryResults->startNs
                                                  : (now > INTERVAL ? now - INTERVAL : 0);
            uint64_t intervalEnd = queryResults ? queryResults->endNs : now;

            std::string symbol(_symbols[locate], MessageFieldSizes::STOCK_SIZE);
            stripWhitespaceFromCPPString(symbol);

            output << symbol << "," << vwap << ","
                   << toTimeString(intervalStart, ':') << ","
                   << toTimeString(intervalEnd, ':') << "\n";
        }
    }

    // ======================== TWAP Output Helper Function ========================
    void maybeOutputQueryForLastNMinutesTWAPForEachStock(uint64_t now, const uint64_t NMINUTES) {
        if(!shouldEmitTWAPSnapshot(now, NMINUTES)) return;
        const int FAIL = -1;
        const uint64_t INTERVAL = static_cast<uint64_t>(NMINUTES) * TWAPConstants::NANOSECOND_PER_MINUTE;

        std::cout << "Outputting last " << NMINUTES << " minutes twap at " << toTimeString(now, ':') << std::endl;

        std::filesystem::path outputDir{"../../LiveOutput/twap"};
        std::error_code ec;
        std::filesystem::create_directories(outputDir, ec);
        if(ec) {
            std::cerr << "Failed to ensure TWAP output directory exists: " << ec.message() << '\n';
            return;
        }

        auto filename = std::string("twap_") + toTimeString(now, '_') + ".csv";
        std::ofstream output(outputDir / filename, std::ios::out | std::ios::trunc);
        if(!output.is_open()) {
            std::cerr << "Failed to open TWAP output file: " << filename << '\n';
            return;
        }

        output << "ticker,twap,start,end\n";
        for(size_t locate = 0; locate < _locateIndexedPerStockState.size(); ++locate) {
            PerStockState& state = _locateIndexedPerStockState[locate];
            if(state.twap.totalTime == 0) continue; // No trading activity captured yet
            auto queryResults = state.queryLastNMinutesTWAP(now, NMINUTES);
            double twap = queryResults == std::nullopt ? FAIL : twapAsDouble(queryResults.value());

            uint64_t intervalStart = queryResults ? queryResults->startNs
                                                  : (now > INTERVAL ? now - INTERVAL : 0);
            uint64_t intervalEnd = queryResults ? queryResults->endNs : now;

            std::string symbol(_symbols[locate], MessageFieldSizes::STOCK_SIZE);
            stripWhitespaceFromCPPString(symbol);

            output << symbol << "," << twap << ","
                   << toTimeString(intervalStart, ':') << ","
                   << toTimeString(intervalEnd, ':') << "\n";
        }
    }

    // ======================== RV Output Helper Function ========================
    void maybeOutputQueryForLastNMinutesRVForEachStock(uint64_t now, const uint64_t NMINUTES) {
        if(!shouldEmitRVSnapshot(now, NMINUTES)) return;
        const int FAIL = -1;
        const uint64_t INTERVAL = static_cast<uint64_t>(NMINUTES) * PerStockRVConstants::NANOSECOND_PER_MINUTE;

        std::cout << "Outputting last " << NMINUTES << " minutes realized variance at " << toTimeString(now, ':') << std::endl;

        std::filesystem::path outputDir{"../../LiveOutput/rv"};
        std::error_code ec;
        std::filesystem::create_directories(outputDir, ec);
        if(ec) {
            std::cerr << "Failed to ensure RV output directory exists: " << ec.message() << '\n';
            return;
        }

        auto filename = std::string("rv_") + toTimeString(now, '_') + ".csv";
        std::ofstream output(outputDir / filename, std::ios::out | std::ios::trunc);
        if(!output.is_open()) {
            std::cerr << "Failed to open RV output file: " << filename << '\n';
            return;
        }

        output << "ticker,rv,start,end\n";
        for(size_t locate = 0; locate < _locateIndexedPerStockState.size(); ++locate) {
            PerStockState& state = _locateIndexedPerStockState[locate];
            if(!state.rv.hasLastPrice) continue; // No trading activity captured yet
            auto queryResults = state.queryLastNMinutesRV(now, NMINUTES);
            double rv = queryResults == std::nullopt ? FAIL : queryResults.value().deltaRVSum;
        
            uint64_t intervalStart = queryResults ? queryResults->startNs
                                                  : (now > INTERVAL ? now - INTERVAL : 0);
            uint64_t intervalEnd = queryResults ? queryResults->endNs : now;

            std::string symbol(_symbols[locate], MessageFieldSizes::STOCK_SIZE);
            stripWhitespaceFromCPPString(symbol);

            output << symbol << "," << rv << ","
                   << toTimeString(intervalStart, ':') << ","
                   << toTimeString(intervalEnd, ':') << "\n";
        }
    }

    private:
    // Shard consumer objects
    std::array<ShardConsumer, numberOfShards> _shards;

    // One thread per-shard
    std::array<std::thread, numberOfShards> _threads;

    // One queue per-shard
    std::array<queue_type, numberOfShards> _queues;

    LocateIndexedPerStockState _locateIndexedPerStockState;

    uint64_t _lastVWAPIntervalEmitted = 0;
    uint64_t _lastTWAPIntervalEmitted = 0;
    uint64_t _lastRVIntervalEmitted = 0;
    CLIArgs _args;

    std::array<char[MessageFieldSizes::STOCK_SIZE], PerStockOrderStateConstants::NUM_OF_STOCK_LOCATE> _symbols;
    ankerl::unordered_dense::map<std::string, uint16_t> _locates;

    SystemEventSequence _systemEventSequenceTracker;

    template<size_t... I>
    static std::array<queue_type, numberOfShards> make_queues(std::index_sequence<I...>) {
        return { ((void)I, queue_type{rigtorp::SPSCQueue<BinaryMessageWrapper>(shardSize)})... };
    }

    /**
     * Should be incremented by two, per consumer thread. For MY MACHINE:
     * Main thread: cpu 2
     * Consumer 1: cpu 4
     * Consumer N: 2*N + 4
     * My machine has 8 hyperthreaded core families, so 5 - 7 consumers seems optimal 
     */
    int coreIndex;


    // ======================== VWAP Output Helper Function ========================
    bool shouldEmitVWAPSnapshot(uint64_t now, uint64_t INTERVAL_IN_MINUTES) {
        constexpr uint64_t START_OF_TRADING_DAY_NS = 570ULL * VWAPConstants::NANOSECOND_PER_MINUTE; // 9:30 AM
        constexpr uint64_t END_OF_TRADING_DAY_NS = 960ULL * VWAPConstants::NANOSECOND_PER_MINUTE; // 4:00 PM
        uint64_t INTERVAL_NS = INTERVAL_IN_MINUTES * VWAPConstants::NANOSECOND_PER_MINUTE;
        if(now < START_OF_TRADING_DAY_NS || now > END_OF_TRADING_DAY_NS) return false;

        uint64_t intervalIndex = (now - START_OF_TRADING_DAY_NS) / INTERVAL_NS;
        if(intervalIndex <= _lastVWAPIntervalEmitted) return false;

        _lastVWAPIntervalEmitted = intervalIndex;
        return true;
    }

    // ======================== TWAP Output Helper Function ========================
    bool shouldEmitTWAPSnapshot(uint64_t now, uint64_t INTERVAL_IN_MINUTES) {
        constexpr uint64_t START_OF_TRADING_DAY_NS = 570ULL * TWAPConstants::NANOSECOND_PER_MINUTE; // 9:30 AM
        constexpr uint64_t END_OF_TRADING_DAY_NS = 960ULL * TWAPConstants::NANOSECOND_PER_MINUTE; // 4:00 PM
        uint64_t INTERVAL_NS = INTERVAL_IN_MINUTES * TWAPConstants::NANOSECOND_PER_MINUTE;
        if(now < START_OF_TRADING_DAY_NS || now > END_OF_TRADING_DAY_NS) return false;

        uint64_t intervalIndex = (now - START_OF_TRADING_DAY_NS) / INTERVAL_NS;
        if(intervalIndex <= _lastTWAPIntervalEmitted) return false;

        _lastTWAPIntervalEmitted = intervalIndex;
        return true;
    }

    // ======================== RV Output Helper Function ========================
    bool shouldEmitRVSnapshot(uint64_t now, uint64_t INTERVAL_IN_MINUTES) {
        constexpr uint64_t START_OF_TRADING_DAY_NS = 570ULL * PerStockRVConstants::NANOSECOND_PER_MINUTE; // 9:30 AM
        constexpr uint64_t END_OF_TRADING_DAY_NS = 960ULL * PerStockRVConstants::NANOSECOND_PER_MINUTE; // 4:00 PM
        uint64_t INTERVAL_NS = INTERVAL_IN_MINUTES * PerStockRVConstants::NANOSECOND_PER_MINUTE;
        if(now < START_OF_TRADING_DAY_NS || now > END_OF_TRADING_DAY_NS) return false;

        uint64_t intervalIndex = (now - START_OF_TRADING_DAY_NS) / INTERVAL_NS;
        if(intervalIndex <= _lastRVIntervalEmitted) return false;

        _lastRVIntervalEmitted = intervalIndex;
        return true;
    }
};

#endif // SHARD_MANAGER_H_
