#ifndef NASDAQ_BOOKKEEPING_VWAP_MANAGER_H
#define NASDAQ_BOOKKEEPING_VWAP_MANAGER_H

#include <cstdint>
#include <unordered_map>
#include <array>
#include <cmath>
#include <string>
#include <memory>

#include <time_utils.h>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/basic_file_sink.h"

constexpr uint32_t TOP_OF_10_PERCENT_BAND = 250000;
constexpr uint32_t TOP_OF_5_PERCENT_BAND  = 500000;

/**
 * This class contains data for VWAP calculation and derives it.
 */
class VWAPFormula {

    private:
        uint32_t high = 0;
        uint32_t low = UINT32_MAX;
        uint32_t close = 0;
        uint64_t closingTimestamp = 0;
        uint64_t volume = 0;
        uint32_t lastSalePrice = UINT32_MAX; // Use for evaluating possible erroneous trade

    public:

        VWAPFormula() = default;
        VWAPFormula(const VWAPFormula& other) = default;
        VWAPFormula& operator=(const VWAPFormula& other) = default;
        VWAPFormula(VWAPFormula&& other) noexcept = default;
        VWAPFormula& operator=(VWAPFormula&& other) noexcept = default;
        ~VWAPFormula() = default;

        // Setters
        void setHigh(uint32_t high) { this -> high = high; }
        void setLow(uint32_t low) { this -> low = low; }
        void setClose(uint32_t close) { this -> close = close; }
        void setClosingTimestamp(uint64_t closingTimestamp) { this -> closingTimestamp = closingTimestamp; }
        void setVolume(uint64_t volume) { this -> volume = volume; }
        void setLastSalePrice(uint32_t lastSalePrice) { this -> lastSalePrice = lastSalePrice; }

        // Getters
        uint32_t getHigh() const { return this -> high; }
        uint32_t getLow() const { return this -> low; }
        uint32_t getClose() const { return this -> close; }
        uint64_t getClosingTimestamp() const { return this -> closingTimestamp; }
        uint64_t getVolume() const { return this -> volume; }
        uint32_t getLastSalePrice() const { return this -> lastSalePrice; }

        uint32_t getTypicalPrice() { return std::round((high + low + close) / 3); }
        uint64_t getPV() { return getTypicalPrice() * volume; }


};

/**
 * This class contains the information we need to adjust VWAP after accounting for broken trades
 */
class BrokenTradeCandidate {
    private:
        uint32_t executionPrice;
        uint64_t executionTimestamp;
        uint64_t executionVolume;
        uint16_t stockLocate;

    public:
        BrokenTradeCandidate(uint32_t executionPrice, uint64_t executionTimestamp, uint64_t executionVolume, uint16_t stockLocate)
            : executionPrice(executionPrice),
              executionTimestamp(executionTimestamp),
              executionVolume(executionVolume),
              stockLocate(stockLocate) {}
    
        BrokenTradeCandidate() = default;
        BrokenTradeCandidate(const BrokenTradeCandidate& other) = default;
        BrokenTradeCandidate& operator=(const BrokenTradeCandidate& other) = default;
        BrokenTradeCandidate(BrokenTradeCandidate&& other) noexcept = default;
        BrokenTradeCandidate& operator=(BrokenTradeCandidate&& other) noexcept = default;
        ~BrokenTradeCandidate() = default;

        uint32_t getExecutionPrice() const { return this -> executionPrice; }
        uint64_t getExecutionTimestamp() const { return this -> executionTimestamp; }
        uint64_t getExecutionVolume() const { return this -> executionVolume; }
        uint16_t getStockLocate() const { return this -> stockLocate; }

        void setExecutionPrice(uint32_t executionPrice) { this -> executionPrice = executionPrice; }
        void setExecutionTimestamp(uint64_t executionTimestamp) { this -> executionTimestamp = executionTimestamp; }
        void setExecutionVolume(uint64_t executionVolume) { this -> executionVolume = executionVolume; }
        void setStockLocate(uint16_t stockLocate) { this -> stockLocate = stockLocate; }
};

/**
 * This class manages VWAP calculation for each issue
 */
class VWAPManager {
    public:

    static VWAPManager& getInstance();

    // Highest level function call for dumping periodic VWAP to a file
    void outputBrokenTradeAdjustedVWAP();

    void handleBrokenTrade(uint16_t stockLocate, uint64_t matchNumber);
    void handleCrossTrade(uint64_t timestamp, uint16_t stockLocate, uint32_t crossPrice, uint64_t shares, uint64_t matchNumber);
    void handleOrderExecuted(uint64_t timestamp, uint16_t stockLocate, uint64_t orderReferenceNumber, uint32_t executedShares, uint64_t matchNumber);
    void handleOrderExecutedWithPrice(
        uint64_t timestamp, 
        uint16_t stockLocate, 
        uint64_t orderReferenceNumber,
        uint32_t executedShares,
        uint64_t matchNumber,
        char printable,
        uint32_t executionPrice
    );
    void handleStockTradingAction(uint16_t stockLocate, const std::string& stock, const char tradingState);
    void handleTrade(uint64_t timestamp, uint16_t stockLocate, uint32_t price, uint32_t shares, uint64_t matchNumber);

    private:

    static VWAPManager* _instance;

    // Contains a mapping for stockLocate -> stock symbol
    std::unordered_map<uint16_t, std::string> _stockLocateToStockSymbol;

    // Track the VWAP for each issue throughout the day
    std::unordered_map<uint16_t, std::array<VWAPFormula, NUMBER_OF_PERIODS_PER_DAY>> _periodicVWAPStatsPerIssue;

    /**
     * Keep a mapping of possible broken trades based on the NASDAQ guidelines.
     * At the end of System Hours, this mapping will be "merged" with _periodicVWAPStatsPerIssue to get adjusted VWAP
     */
    std::unordered_map<uint64_t, BrokenTradeCandidate> _brokenTradeCandidates;

    // Merge the two mappings above
    void _mergeRemainingBrokenTradeCandidatesIntoVWAPStatsPerPeriod();

    void _handleOrderOrTradeExecuted(uint16_t stockLocate, uint32_t price, uint64_t volume, uint64_t timestamp, uint64_t matchNumber);
    bool _flagOrderOrTradeAsErroneousCandidate(uint32_t price, uint32_t lastPrice);
    std::array<std::shared_ptr<spdlog::logger>, NUMBER_OF_PERIODS_PER_DAY> _getOutputLoggers();

    double _getPriceFromInt(uint32_t price);
};

#endif // NASDAQ_BOOKKEEPING_VWAP_MANAGER_H