#ifndef MESSAGE_CONSTANTS_H_
#define MESSAGE_CONSTANTS_H_

#include <cstddef>

constexpr size_t NUMBER_OF_BYTES_FOR_HEADER_CHUNK          = 13;
constexpr size_t NUMBER_OF_BYTES_OFFSET_FOR_HEADER_CHUNK   = 2;
constexpr size_t MAX_BUFFER_SIZE = 48;

namespace MessageFieldSizes {

    constexpr size_t MESSAGE_TYPE_SIZE = 1;
    constexpr size_t STOCK_LOCATE_SIZE = 2;
    constexpr size_t TRACKING_NUMBER_SIZE = 2;
    constexpr size_t TIMESTAMP_SIZE = 6;

    constexpr size_t ORDER_REFERENCE_NUMBER_SIZE = 8;
    constexpr size_t BUY_SELL_INDICATOR_SIZE = 1;
    constexpr size_t STOCK_SIZE = 8;
    constexpr size_t SHARES_SIZE = 4;
    constexpr size_t PRICE_SIZE = 4;
    constexpr size_t ATTRIBUTION_SIZE = 4;
    constexpr size_t MATCH_NUMBER_SIZE = 8;
    constexpr size_t CROSS_PRICE_SIZE = 4;
    constexpr size_t SHARES_IN_TRADE_MESSAGE_SIZE = 8;
    constexpr size_t CANCELLED_SHARES_SIZE = 4;
    constexpr size_t EXECUTED_SHARES_SIZE = 4;
    constexpr size_t PRINTABLE_SIZE = 1;
    constexpr size_t EXECUTION_PRICE_SIZE = 4;
    constexpr size_t TRADING_STATE_SIZE = 1;
    constexpr size_t RESERVED_SIZE = 1;
    constexpr size_t RESASON_SIZE = 4;

};

#endif // MESSAGE_CONSTANTS_H_