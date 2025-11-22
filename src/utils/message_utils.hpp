#ifndef MESSAGE_UTILS_HPP_
#define MESSAGE_UTILS_HPP_

#include <cstddef> // size_t

/**
 * Message Type identifiers for all of the messages from the protocol spec
 */ 
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

/**
 * Message size in bytes for all messages in the protocol spec, EXCLUDING THE HEADER
 */
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

size_t messageTypeToNumberOfBytes(char messageType)
{
    size_t messageSize;
    switch (messageType)
    {
    case MESSAGE_TYPE_SYSTEM_EVENT:
        messageSize = MESSAGE_SIZE_SYSTEM_EVENT;
        break;

    case MESSAGE_TYPE_STOCK_DIRECTORY:
        messageSize = MESSAGE_SIZE_STOCK_DIRECTORY;
        break;
    case MESSAGE_TYPE_STOCK_TRADING_ACTION:
        messageSize = MESSAGE_SIZE_STOCK_TRADING_ACTION;
        break;
    case MESSAGE_TYPE_REG_SHO:
        messageSize = MESSAGE_SIZE_REG_SHO;
        break;
    case MESSAGE_TYPE_MARKET_PARTICIPANT_POSITION:
        messageSize = MESSAGE_SIZE_MARKET_PARTICIPANT_POSITION;
        break;
    case MESSAGE_TYPE_MWCB_DECLINE_LEVEL:
        messageSize = MESSAGE_SIZE_MWCB_DECLINE_LEVEL;
        break;
    case MESSAGE_TYPE_MWCB_STATUS:
        messageSize = MESSAGE_SIZE_MWCB_STATUS;
        break;
    case MESSAGE_TYPE_QUOTING_PERIOD_UPDATE:
        messageSize = MESSAGE_SIZE_QUOTING_PERIOD_UPDATE;
        break;
    case MESSAGE_TYPE_LULD_AUCTION_COLLAR:
        messageSize = MESSAGE_SIZE_LULD_AUCTION_COLLAR;
        break;
    case MESSAGE_TYPE_OPERATIONAL_HALT:
        messageSize = MESSAGE_SIZE_OPERATIONAL_HALT;
        break;

    case MESSAGE_TYPE_ADD_ORDER_NO_MPID:
        messageSize = MESSAGE_SIZE_ADD_ORDER_NO_MPID;
        break;
    case MESSAGE_TYPE_ADD_ORDER_WITH_MPID:
        messageSize = MESSAGE_SIZE_ADD_ORDER_WITH_MPID;
        break;

    case MESSAGE_TYPE_ORDER_EXECUTED:
        messageSize = MESSAGE_SIZE_ORDER_EXECUTED;
        break;
    case MESSAGE_TYPE_ORDER_EXECUTED_WITH_PRICE:
        messageSize = MESSAGE_SIZE_ORDER_EXECUTED_WITH_PRICE;
        break;
    case MESSAGE_TYPE_ORDER_CANCELLED:
        messageSize = MESSAGE_SIZE_ORDER_CANCELLED;
        break;
    case MESSAGE_TYPE_ORDER_DELETE:
        messageSize = MESSAGE_SIZE_ORDER_DELETE;
        break;
    case MESSAGE_TYPE_ORDER_REPLACE:
        messageSize = MESSAGE_SIZE_ORDER_REPLACE;
        break;

    case MESSAGE_TYPE_TRADE_NON_CROSS:
        messageSize = MESSAGE_SIZE_TRADE_NON_CROSS;
        break;
    case MESSAGE_TYPE_TRADE_CROSS:
        messageSize = MESSAGE_SIZE_TRADE_CROSS;
        break;
    case MESSAGE_TYPE_TRADE_BROKEN:
        messageSize = MESSAGE_SIZE_TRADE_BROKEN;
        break;

    case MESSAGE_TYPE_NOII:
        messageSize = MESSAGE_SIZE_NOII;
        break;

    case MESSAGE_TYPE_RPII:
        messageSize = MESSAGE_SIZE_RPII;
        break;

    case MESSAGE_TYPE_DLCR_PRICE_DISCOVERY:
        messageSize = MESSAGE_SIZE_DLCR_PRICE_DISCOVERY;
        break;

    default:
        messageSize = 0;
        break;
    }
    return messageSize;
}

#endif // MESSAGE_UTILS_HPP_