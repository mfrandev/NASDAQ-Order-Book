#ifndef TRADE_CROSS_H_
#define TRADE_CROSS_H_

#include <MessageConstants.h>
#include <endian_utils.hpp>

struct TradeCross {
    char crossType;
    uint32_t crossPrice;
    uint64_t shares;
    uint64_t matchNumber;
    // std::string stock
};

TradeCross parseTradeCrossBody(const char data[MAX_BUFFER_SIZE]) {
    size_t offset = 0;
    uint64_t shares = toHostEndianUpTo64(&data[offset], MessageFieldSizes::SHARES_IN_TRADE_MESSAGE_SIZE);
    offset += MessageFieldSizes::SHARES_IN_TRADE_MESSAGE_SIZE;
    // std::string stock;
    offset += MessageFieldSizes::STOCK_SIZE;
    uint32_t crossPrice = toHostEndianUpTo64(&data[offset], MessageFieldSizes::CROSS_PRICE_SIZE);
    offset += MessageFieldSizes::CROSS_PRICE_SIZE;
    uint64_t matchNumber = toHostEndianUpTo64(&data[offset], MessageFieldSizes::MATCH_NUMBER_SIZE);
    offset += MessageFieldSizes::MATCH_NUMBER_SIZE;
    char crossType = data[offset];
    return TradeCross{crossType, crossPrice, shares, matchNumber};
}

#endif // TRADE_CROSS_H_