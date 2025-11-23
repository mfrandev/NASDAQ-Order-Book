#ifndef TRADE_NON_CROSS_H_
#define TRADE_NON_CROSS_H_

#include <endian_utils.hpp>
#include <MessageConstants.h>

struct TradeNonCross {

    char buySellIndicator;
    uint32_t shares;
    uint32_t price;
    uint64_t orderReferenceNumber;
    uint64_t matchNumber;

};

TradeNonCross parseTradeNonCrossBody(const char* data) {
    size_t offset = 0;
    uint64_t orderReferenceNumber = toHostEndianUpTo64(&data[offset], MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE); // We know this is an 8 byte int
    offset += MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE;
    char buySellIndicator = data[offset];
    offset += MessageFieldSizes::BUY_SELL_INDICATOR_SIZE;
    uint32_t shares = toHostEndianUpTo64(&data[offset], MessageFieldSizes::SHARES_SIZE); // We know this is a 4 byte int
    offset += MessageFieldSizes::SHARES_SIZE;
    // std::string stock = std::string(&data[offset], MessageFieldSizes::STOCK_SIZE);
    // stripWhitespaceFromCPPString(stock);
    offset += MessageFieldSizes::STOCK_SIZE;
    uint32_t price = toHostEndianUpTo64(&data[offset], MessageFieldSizes::PRICE_SIZE); // We know this is a 4 byte int
    offset += MessageFieldSizes::PRICE_SIZE;
    uint64_t matchNumber = toHostEndianUpTo64(&data[offset], MessageFieldSizes::MATCH_NUMBER_SIZE); // We know this is an 8 byte int
    return TradeNonCross{buySellIndicator, shares, price, orderReferenceNumber, matchNumber};
}

#endif // TRADE_NON_CROSS_H_