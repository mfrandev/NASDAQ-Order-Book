#ifndef SHARD_MANAGER_H_
#define SHARD_MANAGER_H_

#include <utility>
#include <array>
#include <iostream>

#include <ShardConsumer.hpp>
#include <VWAPQueryResults.hpp>
#include <BinaryMessageWrapper.h>
#include <PerStockOrderState.h>

#include <message_utils.hpp>

#include <ProcessSystemEvent.hpp>

template<uint8_t numberOfShards, uint32_t shardSize>
class ShardManager {

    public:
    ShardManager() : 
        _queues(make_queues(std::make_index_sequence<numberOfShards>{})), 
        coreIndex(ThreadConstants::FIRST_CONSUMER_CORE)
        {
            _locates.reserve(16'000UL);
        }

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

    std::vector<std::vector<double>> queryDayAndLastOneAndLastFiveMintues(std::vector<std::string> symbols) {
        std::vector<std::vector<double>> result;
        const uint64_t EOD = 57600000000000;
        // const uint64_t SOD = 34200000000000;
        for(auto& symbol: symbols) {
            uint16_t stockLocate = _locates.at(symbol);
            std::vector<double> oneFiveDayVWAPResults; 
            auto one = _locateIndexedPerStockState[stockLocate].queryLastNMinutesVWAP(EOD, 2);
            auto five = _locateIndexedPerStockState[stockLocate].queryLastNMinutesVWAP(EOD, 6);
            auto day = _locateIndexedPerStockState[stockLocate].queryLastNMinutesVWAP(EOD, 391);
            oneFiveDayVWAPResults.push_back(one == std::nullopt ? 0.d : vwapAsDouble(one.value()));
            oneFiveDayVWAPResults.push_back(five == std::nullopt ? 0.d : vwapAsDouble(five.value()));
            oneFiveDayVWAPResults.push_back(day == std::nullopt ? 0.d : vwapAsDouble(day.value()));
            result.push_back(oneFiveDayVWAPResults);
        }
        return result;
    }

    private:
    // Shard consumer objects
    std::array<ShardConsumer, numberOfShards> _shards;

    // One thread per-shard
    std::array<std::thread, numberOfShards> _threads;

    // One queue per-shard
    std::array<queue_type, numberOfShards> _queues;

    LocateIndexedPerStockState _locateIndexedPerStockState;

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

};

#endif // SHARD_MANAGER_H_
