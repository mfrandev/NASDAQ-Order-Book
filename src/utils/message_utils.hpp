#ifndef MESSAGE_UTILS_HPP_
#define MESSAGE_UTILS_HPP_

#include <utility>

#include <murmur3.h> // Peter Scott C implementation for uniform hashing

#include <MessageConstants.h>

constexpr uint32_t HASH_SEED = 12721; // Nice 5-digit palendromic prime

uint32_t getShardIndex(uint16_t stockLocate) {
    uint32_t index = 0;
    MurmurHash3_x86_32(&stockLocate, sizeof(uint16_t), HASH_SEED, &index);
    return index % NUMBER_OF_SHARDS;
}

size_t messageTypeToNumberOfBytes(char messageType)
{
    size_t messageSize;
    switch (messageType)
    {
    case MessageTypes::MESSAGE_TYPE_SYSTEM_EVENT:
        messageSize = MessageSizes::MESSAGE_SIZE_SYSTEM_EVENT;
        break;

    case MessageTypes::MESSAGE_TYPE_STOCK_DIRECTORY:
        messageSize = MessageSizes::MESSAGE_SIZE_STOCK_DIRECTORY;
        break;
    case MessageTypes::MESSAGE_TYPE_STOCK_TRADING_ACTION:
        messageSize = MessageSizes::MESSAGE_SIZE_STOCK_TRADING_ACTION;
        break;
    case MessageTypes::MESSAGE_TYPE_REG_SHO:
        messageSize = MessageSizes::MESSAGE_SIZE_REG_SHO;
        break;
    case MessageTypes::MESSAGE_TYPE_MARKET_PARTICIPANT_POSITION:
        messageSize = MessageSizes::MESSAGE_SIZE_MARKET_PARTICIPANT_POSITION;
        break;
    case MessageTypes::MESSAGE_TYPE_MWCB_DECLINE_LEVEL:
        messageSize = MessageSizes::MESSAGE_SIZE_MWCB_DECLINE_LEVEL;
        break;
    case MessageTypes::MESSAGE_TYPE_MWCB_STATUS:
        messageSize = MessageSizes::MESSAGE_SIZE_MWCB_STATUS;
        break;
    case MessageTypes::MESSAGE_TYPE_QUOTING_PERIOD_UPDATE:
        messageSize = MessageSizes::MESSAGE_SIZE_QUOTING_PERIOD_UPDATE;
        break;
    case MessageTypes::MESSAGE_TYPE_LULD_AUCTION_COLLAR:
        messageSize = MessageSizes::MESSAGE_SIZE_LULD_AUCTION_COLLAR;
        break;
    case MessageTypes::MESSAGE_TYPE_OPERATIONAL_HALT:
        messageSize = MessageSizes::MESSAGE_SIZE_OPERATIONAL_HALT;
        break;

    case MessageTypes::MESSAGE_TYPE_ADD_ORDER_NO_MPID:
        messageSize = MessageSizes::MESSAGE_SIZE_ADD_ORDER_NO_MPID;
        break;
    case MessageTypes::MESSAGE_TYPE_ADD_ORDER_WITH_MPID:
        messageSize = MessageSizes::MESSAGE_SIZE_ADD_ORDER_WITH_MPID;
        break;

    case MessageTypes::MESSAGE_TYPE_ORDER_EXECUTED:
        messageSize = MessageSizes::MESSAGE_SIZE_ORDER_EXECUTED;
        break;
    case MessageTypes::MESSAGE_TYPE_ORDER_EXECUTED_WITH_PRICE:
        messageSize = MessageSizes::MESSAGE_SIZE_ORDER_EXECUTED_WITH_PRICE;
        break;
    case MessageTypes::MESSAGE_TYPE_ORDER_CANCELLED:
        messageSize = MessageSizes::MESSAGE_SIZE_ORDER_CANCELLED;
        break;
    case MessageTypes::MESSAGE_TYPE_ORDER_DELETE:
        messageSize = MessageSizes::MESSAGE_SIZE_ORDER_DELETE;
        break;
    case MessageTypes::MESSAGE_TYPE_ORDER_REPLACE:
        messageSize = MessageSizes::MESSAGE_SIZE_ORDER_REPLACE;
        break;

    case MessageTypes::MESSAGE_TYPE_TRADE_NON_CROSS:
        messageSize = MessageSizes::MESSAGE_SIZE_TRADE_NON_CROSS;
        break;
    case MessageTypes::MESSAGE_TYPE_TRADE_CROSS:
        messageSize = MessageSizes::MESSAGE_SIZE_TRADE_CROSS;
        break;
    case MessageTypes::MESSAGE_TYPE_TRADE_BROKEN:
        messageSize = MessageSizes::MESSAGE_SIZE_TRADE_BROKEN;
        break;

    case MessageTypes::MESSAGE_TYPE_NOII:
        messageSize = MessageSizes::MESSAGE_SIZE_NOII;
        break;

    case MessageTypes::MESSAGE_TYPE_RPII:
        messageSize = MessageSizes::MESSAGE_SIZE_RPII;
        break;

    case MessageTypes::MESSAGE_TYPE_DLCR_PRICE_DISCOVERY:
        messageSize = MessageSizes::MESSAGE_SIZE_DLCR_PRICE_DISCOVERY;
        break;

    default:
        messageSize = 0;
        break;
    }
    return messageSize;
}

#endif // MESSAGE_UTILS_HPP_