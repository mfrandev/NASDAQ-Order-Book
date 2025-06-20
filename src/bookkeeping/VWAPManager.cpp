#include <VWAPManager.h>

#include <cassert>

#include <OrderBook.h>
#include <ProcessMessage.h>
#include <time_utils.h>

extern uint8_t currentPeriod;

// Singleton declaration and getter
VWAPManager* VWAPManager::_instance = nullptr;
VWAPManager& VWAPManager::getInstance() {
    if(_instance == nullptr) 
        _instance = new VWAPManager();
    return *_instance;
}

constexpr char IGNORE_ORDER_EXECUTED_WITH_PRICE_MESSAGE = 'N'; // Non-printable and we should ignore 
constexpr char STOCK_IS_TRADING                         = 'T';

/**
 * Output the broken trade adjusted periodic VWAP to a set of files
 */
void VWAPManager::outputBrokenTradeAdjustedVWAP() {

    // Get our output objects. Cursory Google search implies this is RAII, so no need to close explcitly
    std::array<std::shared_ptr<spdlog::logger>, NUMBER_OF_PERIODS_PER_DAY> loggers = _getOutputLoggers();

    // Perform the broken trade VWAP adjustment
    _mergeRemainingBrokenTradeCandidatesIntoVWAPStatsPerPeriod();
    for(auto& periodicVWAPstatsForOneIssue: _periodicVWAPStatsPerIssue) {
        std::string& currentStockSymbol = _stockLocateToStockSymbol.at(periodicVWAPstatsForOneIssue.first);

        // Use these two values to calculate VWAP over time
        uint64_t cumulativePVForOneIssue = 0;
        uint64_t cumulativeVolumeForOneIssue = 0;
        // Get cumulative VWAP for this issue
        for(int i = 0; i < NUMBER_OF_PERIODS_PER_DAY; i++) {
            VWAPFormula statsForOnePeriod = periodicVWAPstatsForOneIssue.second[i];
            cumulativePVForOneIssue += statsForOnePeriod.getPV();
            cumulativeVolumeForOneIssue += statsForOnePeriod.getVolume();
            if(cumulativeVolumeForOneIssue == 0) continue; // VWAP undefined if no volume
            uint64_t vwapThisPeriod = cumulativePVForOneIssue / cumulativeVolumeForOneIssue;
            // This line actually commits the stock symbol and VWAP to the file
            loggers[i] -> info("{}: {}", currentStockSymbol, _getPriceFromInt(vwapThisPeriod));
        }
    }
}

/**
 * Remove the broken trade from the _brokenTradeCandidate mapping
 */
void VWAPManager::handleBrokenTrade(uint16_t stockLocate, uint64_t matchNumber) {
    // If the heuristic didn't catch it, nothing we can do
    if(!_brokenTradeCandidates.count(matchNumber)) return;
    _brokenTradeCandidates.erase(matchNumber);
}

/**
 * Handle cross trade message
 */
void VWAPManager::handleCrossTrade(uint64_t timestamp, uint16_t stockLocate, uint32_t crossPrice, uint64_t shares, uint64_t matchNumber) {
    if(shares == 0) return;
    _handleOrderOrTradeExecuted(stockLocate, crossPrice, shares, timestamp, matchNumber);
}

/**
 * Handle order execute message
 */
void VWAPManager::handleOrderExecuted(uint64_t timestamp, uint16_t stockLocate, uint64_t orderReferenceNumber, uint32_t executedShares, uint64_t matchNumber) {
    uint32_t price = OrderBook::getInstance().executeActiveOrder(orderReferenceNumber, executedShares, matchNumber);
    _handleOrderOrTradeExecuted(stockLocate, price, executedShares, timestamp, matchNumber);
}

/**
 * Hande order executed with price message
 */
void VWAPManager::handleOrderExecutedWithPrice(uint64_t timestamp, uint16_t stockLocate, 
    uint64_t orderReferenceNumber,
    uint32_t executedShares,
    uint64_t matchNumber,
    char printable,
    uint32_t executionPrice
) {
    if(printable == IGNORE_ORDER_EXECUTED_WITH_PRICE_MESSAGE) return;
    OrderBook::getInstance().executeActiveOrderWithPrice(orderReferenceNumber, executedShares, matchNumber, executionPrice);
    _handleOrderOrTradeExecuted(stockLocate, executionPrice, executedShares, timestamp, matchNumber);
}

/**
 * 
 */
void VWAPManager::handleStockTradingAction(uint16_t stockLocate, const std::string& stock, const char tradingState) {
    if(!_stockLocateToStockSymbol.count(stockLocate))
        _stockLocateToStockSymbol[stockLocate] = stock;
    if(tradingState != STOCK_IS_TRADING) return;
    if(_periodicVWAPStatsPerIssue.count(stockLocate)) return;

    //Create the default vwap and locate-to-symbol mappings for this issue
    _periodicVWAPStatsPerIssue.emplace(stockLocate, std::array<VWAPFormula, NUMBER_OF_PERIODS_PER_DAY>());
}

/**
 * Handle a normal trade message
 */
void VWAPManager::handleTrade(uint64_t timestamp, uint16_t stockLocate, uint32_t price, uint32_t shares, uint64_t matchNumber) {
    _handleOrderOrTradeExecuted(stockLocate, price, shares, timestamp, matchNumber);
}

// ======================== Private Functions ========================

/**
 * Handle all orders and trades
 */
void VWAPManager::_handleOrderOrTradeExecuted(uint16_t stockLocate, uint32_t price, uint64_t volume, uint64_t timestamp, uint64_t matchNumber) {
    // Check if this order should flagged for erroneous trade
    // assert(_periodicVWAPStatsPerIssue.count(stockLocate));
    if(!_periodicVWAPStatsPerIssue.count(stockLocate)) return;    
    VWAPFormula& data = _periodicVWAPStatsPerIssue.at(stockLocate).at(currentPeriod);
    if(data.getLastSalePrice() != UINT32_MAX && _flagOrderOrTradeAsErroneousCandidate(price, data.getLastSalePrice())) {
        // If not erroneous, this will eventually get merged into the trade statistics 
        _brokenTradeCandidates[matchNumber] = {price, timestamp, volume, stockLocate};
    } else {
        if(price > data.getHigh())
            data.setHigh(price);
        else if(price < data.getLow())
            data.setLow(price);
        data.setVolume(data.getVolume() + volume);
        data.setClose(price);
        data.setClosingTimestamp(timestamp);
    }
    data.setLastSalePrice(price);
}

/**
 * "Nasdaq does not normally break trades that are between the Reference Price and up to but not including the Numerical Threshold."
 * https://www.nasdaqtrader.com/Trader.aspx?id=ClearlyErroneous#:~:text=The%20terms%20of%20a%20transaction,or%20identification%20of%20the%20security 
 * Using their values as a heuristic to 
 */
bool VWAPManager::_flagOrderOrTradeAsErroneousCandidate(uint32_t price, uint32_t lastPrice) {
    double lowerBound = 0;
    double upperBound = 0;

    // Nasdaq advises these are the cutoff points
    if(price < TOP_OF_10_PERCENT_BAND) {
        lowerBound = 0.9;
        upperBound = 1.1;
    } else if(price < TOP_OF_5_PERCENT_BAND) {
        lowerBound = 0.95;
        upperBound = 1.05;
    } else {
        lowerBound = 0.97;
        upperBound = 1.03;
    }
    uint32_t floor = lastPrice * lowerBound;
    uint32_t ceiling = lastPrice * upperBound;
    return price <= floor || price >= ceiling;
}

/**
 * At the end of system hours, any non-broken, intraday trades should be accounted for
 */
void VWAPManager::_mergeRemainingBrokenTradeCandidatesIntoVWAPStatsPerPeriod() {
    for(auto& mergeCandidate: _brokenTradeCandidates) {
        uint8_t period = getCurrentPeriodFromTimestamp(mergeCandidate.second.getExecutionTimestamp());
        
        assert(_periodicVWAPStatsPerIssue.count(mergeCandidate.second.getStockLocate()));
        
        // Discount after hours trades. We are only interested in intraday activity
        if(period < 0 || period >= NUMBER_OF_PERIODS_PER_DAY) {
        //     fmt::println("Found erroneous out-of-hours trade: Timestamp: {} is period: {} Price: {} Volume: {} Stock Locate: {}", mergeCandidate.second.executionTimestamp, 
        //         period, 
        //         mergeCandidate.second.executionPrice, 
        //         mergeCandidate.second.executionVolume, 
        //         mergeCandidate.second.stockLocate);
            continue;
        }
        assert(period < NUMBER_OF_PERIODS_PER_DAY);
        VWAPFormula dataToUpdate = _periodicVWAPStatsPerIssue[mergeCandidate.second.getStockLocate()][period];

        // Perform the merge
        if(mergeCandidate.second.getExecutionPrice() < dataToUpdate.getLow())
            dataToUpdate.setLow(mergeCandidate.second.getExecutionPrice());
        else if(mergeCandidate.second.getExecutionPrice() > dataToUpdate.getHigh())
            dataToUpdate.setHigh(mergeCandidate.second.getExecutionPrice());
        dataToUpdate.setVolume(dataToUpdate.getVolume() + mergeCandidate.second.getExecutionVolume());
        // No need to worry about the closing stats if the merge candidate is not later in the period than the currently tracked last trade
        if(mergeCandidate.second.getExecutionTimestamp() < dataToUpdate.getClosingTimestamp()) 
            continue;
        dataToUpdate.setClosingTimestamp(mergeCandidate.second.getExecutionTimestamp());
        dataToUpdate.setClose(mergeCandidate.second.getExecutionPrice());
    }

    // Free the memory from _brokenTradeCandidates after the merge is complete
    std::unordered_map<uint64_t, BrokenTradeCandidate> temp;
    temp.swap(_brokenTradeCandidates);
}   

std::array<std::shared_ptr<spdlog::logger>, NUMBER_OF_PERIODS_PER_DAY> VWAPManager::_getOutputLoggers() {
    std::array<std::shared_ptr<spdlog::logger>, NUMBER_OF_PERIODS_PER_DAY> outputLoggers = {
        spdlog::basic_logger_mt("period_1_logger", "./../../LiveOutput/vwap_1.txt"),
        spdlog::basic_logger_mt("period_2_logger", "./../../LiveOutput/vwap_2.txt"),
        spdlog::basic_logger_mt("period_3_logger", "./../../LiveOutput/vwap_3.txt"),
        spdlog::basic_logger_mt("period_4_logger", "./../../LiveOutput/vwap_4.txt"),
        spdlog::basic_logger_mt("period_5_logger", "./../../LiveOutput/vwap_5.txt"),
        spdlog::basic_logger_mt("period_6_logger", "./../../LiveOutput/vwap_6.txt"),
        spdlog::basic_logger_mt("period_7_logger", "./../../LiveOutput/vwap_7.txt")
    };

    return outputLoggers;
}


double VWAPManager::_getPriceFromInt(uint32_t price) {
    return static_cast<double>(static_cast<double>(price) / 10000);
}
