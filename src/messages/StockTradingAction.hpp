#ifndef STOCK_TRADING_ACTION_H_
#define STOCK_TRADING_ACTION_H_

#include <cstdint>

#include <string_utils.hpp>
#include <MessageConstants.h>

struct StockTradingAction {
    char tradingState;
    char reserved;
};

/**
 * TODO: This message is responsible for building the map of tradable stock tickers {stockLocate -> stockTicker}. 
 * Will need to build that map as these messages get processed to bypass the need to store any string data in the message itself.
 */
StockTradingAction parseStockTradingActionBody(const char data[MAX_BUFFER_SIZE]) {
    size_t offset = 0;
    std::string stock = std::string(&data[offset], MessageFieldSizes::STOCK_SIZE);
    stripWhitespaceFromCPPString(stock);
    offset += MessageFieldSizes::STOCK_SIZE;
    char tradingState = data[offset];
    offset += MessageFieldSizes::TRADING_STATE_SIZE;
    char reserved = data[offset];
    offset += MessageFieldSizes::RESERVED_SIZE;
    // std::string reason = std::string(&data[offset], MessageFieldSizes::REASON_SIZE);
    // stripWhitespaceFromCPPString(reason);
    return StockTradingAction{tradingState, reserved};
}

#endif // STOCK_TRADING_ACTION_H_