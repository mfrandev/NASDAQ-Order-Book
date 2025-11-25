#ifndef MESSAGE_UNION_H_
#define MESSAGE_UNION_H_

#include <cstdint>

#include <MessageHeader.hpp>
#include <AddOrder.hpp>
#include <AddOrderWithMPID.hpp>
#include <BrokenTradeOrOrderExecution.hpp>
#include <OrderCancel.hpp>
#include <OrderDelete.hpp>
#include <OrderExecuted.hpp>
#include <OrderExecutedWithPrice.hpp>
#include <OrderReplace.hpp>
#include <StockTradingAction.hpp>
#include <SystemEvent.hpp>
#include <TradeCross.hpp>
#include <TradeNonCross.hpp>

enum class MessageTag : uint8_t {
    // Real ITCH Messages
    AddOrder,
    AddOrderWithMPID,
    BrokenTradeOrOrderExecution,
    OrderCancel,
    OrderDelete,
    OrderExecuted,
    OrderExecutedWithPrice,
    OrderReplace,
    StockTradingAction,
    SystemEvent,
    TradeCross,
    TradeNonCross,

    // System signals as MessageTag
    SentinelMessage,
    NullMessage
};

struct MessageBody {

    MessageTag tag;

    union {

        AddOrder addOrder;
        AddOrderWithMPID addOrderWithMPID;
        BrokenTradeOrOrderExecution brokenTradeOrOrderExecution;
        OrderCancel orderCancel;
        OrderDelete orderDelete;
        OrderExecuted orderExecuted;
        OrderExecutedWithPrice orderExecutedWithPrice;
        OrderReplace orderReplace;
        StockTradingAction stockTradingAction;
        SystemEvent systemEvent;
        TradeCross tradeCross;
        TradeNonCross tradeNonCross;

    };

};

struct Message {
    MessageHeader header;
    MessageBody body;
};

MessageBody parseMessage(const char messageType, const uint16_t stockLocate, const char buffer[MAX_BUFFER_SIZE]) {
    switch (messageType) {
        case MessageTypes::MESSAGE_TYPE_ADD_ORDER_NO_MPID:
        return MessageBody {
            .tag = MessageTag::AddOrder,
            .addOrder = parseAddOrderBody(buffer)
        };

        case MessageTypes::MESSAGE_TYPE_ADD_ORDER_WITH_MPID:
        return MessageBody {
            .tag = MessageTag::AddOrderWithMPID, 
            .addOrderWithMPID = parseAddOrderWithMPIDBody(buffer)
        };

        case MessageTypes::MESSAGE_TYPE_TRADE_BROKEN:
        return MessageBody {
            .tag = MessageTag::BrokenTradeOrOrderExecution,
            .brokenTradeOrOrderExecution =parseBrokenTradeOrOrderExecutionBody(buffer)
        };

        case MessageTypes::MESSAGE_TYPE_ORDER_CANCELLED:
        return MessageBody {
            .tag = MessageTag::OrderCancel,
            .orderCancel = parseOrderCancelBody(buffer)
        };

        case MessageTypes::MESSAGE_TYPE_ORDER_DELETE:
        return MessageBody {
            .tag = MessageTag::OrderDelete,
            .orderDelete = parseOrderDeleteBody(buffer)
        };

        case MessageTypes::MESSAGE_TYPE_ORDER_EXECUTED:
        return MessageBody {
            .tag = MessageTag::OrderExecuted,
            .orderExecuted = parseOrderExecutedBody(buffer)
        };

        case MessageTypes::MESSAGE_TYPE_ORDER_EXECUTED_WITH_PRICE:
        return MessageBody {
            .tag = MessageTag::OrderExecutedWithPrice ,
            .orderExecutedWithPrice = parseOrderExecutedWithPriceBody(buffer)
        };

        case MessageTypes::MESSAGE_TYPE_ORDER_REPLACE:
        return MessageBody {
            .tag = MessageTag::OrderReplace,
            .orderReplace = parseOrderReplaceBody(buffer)
        };

        case MessageTypes::MESSAGE_TYPE_STOCK_TRADING_ACTION:
        return MessageBody {
            .tag = MessageTag::StockTradingAction,
            .stockTradingAction = parseStockTradingActionBody(stockLocate, buffer)
        };

        case MessageTypes::MESSAGE_TYPE_SYSTEM_EVENT:
        return MessageBody {
            .tag = MessageTag::SystemEvent,
            .systemEvent = parseSystemEventBody(buffer)
        };

        case MessageTypes::MESSAGE_TYPE_TRADE_CROSS:
        return MessageBody {
            .tag = MessageTag::TradeCross,
            .tradeCross = parseTradeCrossBody(buffer)
        };

        case MessageTypes::MESSAGE_TYPE_TRADE_NON_CROSS:
        return MessageBody {
            .tag = MessageTag::TradeNonCross,
            .tradeNonCross = parseTradeNonCrossBody(buffer)
        };

        default:
        return MessageBody {
          .tag = MessageTag::NullMessage  
        };
    }
}

#endif // MESSASGE_UNION_H_