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
#include <StockDirectory.hpp>
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
    StockDirectory,
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
        StockDirectory stockDirectory;
        StockTradingAction stockTradingAction;
        SystemEvent systemEvent;
        TradeCross tradeCross;
        TradeNonCross tradeNonCross;

    };

};

struct Message {
    uint64_t seq;
    MessageHeader header;
    MessageBody body;
};

static_assert(sizeof(Message) <= 64);

#endif // MESSASGE_UNION_H_