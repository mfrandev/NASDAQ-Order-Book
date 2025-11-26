#ifndef MESSAGE_CONSTANTS_H_
#define MESSAGE_CONSTANTS_H_

#include <cstddef>

constexpr size_t NUMBER_OF_BYTES_FOR_HEADER_CHUNK          = 13;
constexpr size_t NUMBER_OF_BYTES_OFFSET_FOR_HEADER_CHUNK   = 2;
constexpr size_t MAX_BUFFER_SIZE = 48;

constexpr char PRINTABLE = 'Y';
constexpr char NON_PRINTABLE = 'N';

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

/**
 * Message Type identifiers for all of the messages from the protocol spec
 */ 
namespace MessageTypes {
    constexpr char MESSAGE_TYPE_SYSTEM_EVENT                = 'S'; // 1.1

    constexpr char MESSAGE_TYPE_STOCK_DIRECTORY             = 'R'; // 1.2.1
    constexpr char MESSAGE_TYPE_STOCK_TRADING_ACTION        = 'H'; // 1.2.2
    constexpr char MESSAGE_TYPE_REG_SHO                     = 'Y'; // 1.2.3
    constexpr char MESSAGE_TYPE_MARKET_PARTICIPANT_POSITION = 'L'; // 1.2.4
    constexpr char MESSAGE_TYPE_MWCB_DECLINE_LEVEL          = 'V'; // 1.2.5.1
    constexpr char MESSAGE_TYPE_MWCB_STATUS                 = 'W'; // 1.2.5.2
    constexpr char MESSAGE_TYPE_QUOTING_PERIOD_UPDATE       = 'K'; // 1.2.6
    constexpr char MESSAGE_TYPE_LULD_AUCTION_COLLAR         = 'J'; // 1.2.7
    constexpr char MESSAGE_TYPE_OPERATIONAL_HALT            = 'h'; // 1.2.8

    constexpr char MESSAGE_TYPE_ADD_ORDER_NO_MPID           = 'A'; // 1.3.1
    constexpr char MESSAGE_TYPE_ADD_ORDER_WITH_MPID         = 'F'; // 1.3.2

    constexpr char MESSAGE_TYPE_ORDER_EXECUTED              = 'E'; // 1.4.1
    constexpr char MESSAGE_TYPE_ORDER_EXECUTED_WITH_PRICE   = 'C'; // 1.4.2
    constexpr char MESSAGE_TYPE_ORDER_CANCELLED             = 'X'; // 1.4.3
    constexpr char MESSAGE_TYPE_ORDER_DELETE                = 'D'; // 1.4.4
    constexpr char MESSAGE_TYPE_ORDER_REPLACE               = 'U'; // 1.4.5

    constexpr char MESSAGE_TYPE_TRADE_NON_CROSS             = 'P'; // 1,5,1
    constexpr char MESSAGE_TYPE_TRADE_CROSS                 = 'Q'; // 1.5.2
    constexpr char MESSAGE_TYPE_TRADE_BROKEN                = 'B'; // 1.5.3

    constexpr char MESSAGE_TYPE_NOII                        = 'I'; // 1.6

    constexpr char MESSAGE_TYPE_RPII                        = 'N'; // 1.7

    constexpr char MESSAGE_TYPE_DLCR_PRICE_DISCOVERY        = 'O'; // 1.8

    constexpr char MESSAGE_TYPE_SENTINEL_MESSAGE            = '1'; // Terminate consumer thread
};

/**
 * Message size in bytes for all messages in the protocol spec, EXCLUDING THE HEADER
 */
namespace MessageSizes {
    constexpr size_t MESSAGE_HEADER_SIZE                      = 11; // COMMON MESSAGE HEADER

    constexpr size_t MESSAGE_SIZE_SYSTEM_EVENT                = 1; // 1.1

    constexpr size_t MESSAGE_SIZE_STOCK_DIRECTORY             = 28; // 1.2.1
    constexpr size_t MESSAGE_SIZE_STOCK_TRADING_ACTION        = 14; // 1.2.2
    constexpr size_t MESSAGE_SIZE_REG_SHO                     = 9; // 1.2.3
    constexpr size_t MESSAGE_SIZE_MARKET_PARTICIPANT_POSITION = 15; // 1.2.4
    constexpr size_t MESSAGE_SIZE_MWCB_DECLINE_LEVEL          = 24; // 1.2.5.1
    constexpr size_t MESSAGE_SIZE_MWCB_STATUS                 = 1; // 1.2.5.2
    constexpr size_t MESSAGE_SIZE_QUOTING_PERIOD_UPDATE       = 17; // 1.2.6
    constexpr size_t MESSAGE_SIZE_LULD_AUCTION_COLLAR         = 24; // 1.2.7
    constexpr size_t MESSAGE_SIZE_OPERATIONAL_HALT            = 10; // 1.2.8

    constexpr size_t MESSAGE_SIZE_ADD_ORDER_NO_MPID           = 25; // 1.3.1
    constexpr size_t MESSAGE_SIZE_ADD_ORDER_WITH_MPID         = 29; // 1.3.2

    constexpr size_t MESSAGE_SIZE_ORDER_EXECUTED              = 20; // 1.4.1
    constexpr size_t MESSAGE_SIZE_ORDER_EXECUTED_WITH_PRICE   = 25; // 1.4.2
    constexpr size_t MESSAGE_SIZE_ORDER_CANCELLED             = 12; // 1.4.3
    constexpr size_t MESSAGE_SIZE_ORDER_DELETE                = 8; // 1.4.4
    constexpr size_t MESSAGE_SIZE_ORDER_REPLACE               = 24; // 1.4.5

    constexpr size_t MESSAGE_SIZE_TRADE_NON_CROSS             = 33; // 1,5,1
    constexpr size_t MESSAGE_SIZE_TRADE_CROSS                 = 29; // 1.5.2
    constexpr size_t MESSAGE_SIZE_TRADE_BROKEN                = 8; // 1.5.3

    constexpr size_t MESSAGE_SIZE_NOII                        = 39; // 1.6

    constexpr size_t MESSAGE_SIZE_RPII                        = 9; // 1.7

    constexpr size_t MESSAGE_SIZE_DLCR_PRICE_DISCOVERY        = 37; // 1.8
};

#endif // MESSAGE_CONSTANTS_H_