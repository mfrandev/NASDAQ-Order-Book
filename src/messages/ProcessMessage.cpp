#include <ProcessMessage.h>

#include <spdlog/spdlog.h>

#include <cassert>

#include <Message.h>
#include <AddOrder.h>
#include <AddOrderMPID.h>
#include <BrokenTradeOrOrderExecution.h>
#include <CrossTrade.h>
#include <OrderCancel.h>
#include <OrderDelete.h>
#include <OrderExecuted.h>
#include <OrderExecutedWithPrice.h>
#include <OrderReplace.h>
#include <StockTradingAction.h>
#include <SystemEvent.h>
#include <TradeNonCross.h>

#include <time_utils.h>

extern uint8_t currentPeriod;
extern const std::string& CONSOLE_LOGGER;

void ProcessMessage::processHeaderTimestamp(uint64_t timestamp) {

    // Check if the hour has changed and handle things appropriately
    uint8_t periodOfThisMessage = getCurrentPeriodFromTimestamp(timestamp);
    if (periodOfThisMessage != currentPeriod)
    {

        // No need to continue keeping track of reporting period if market hours are done
        if (periodOfThisMessage < NUMBER_OF_PERIODS_PER_DAY) {
            spdlog::get(CONSOLE_LOGGER) -> info("Timestamp: {} Moving from period {} to period {}", timestamp, currentPeriod, periodOfThisMessage);
            currentPeriod = periodOfThisMessage;
        }
    }
}

Message* ProcessMessage::getMessage(const char *data, size_t bytesToRead, BinaryMessageHeader header)
{

    switch (header.getMessageType())
    {
    case MESSAGE_TYPE_ADD_ORDER_NO_MPID:
    {
        return parseAddOrderBody(std::move(header), data);
        // spdlog::get(CONSOLE_LOGGER) -> info("1. Adding: {},{},{},{},{}", addOrder.getOrderReferenceNumber(), addOrder.getBuySellIndicator(), addOrder.getShares(), addOrder.getStock(), addOrder.getPrice());
    }
    break;
    case MESSAGE_TYPE_ADD_ORDER_WITH_MPID:
    {
        return parseAddOrderMPIDBody(std::move(header), data);
        // spdlog::get(CONSOLE_LOGGER) -> info("2. Adding: {},{},{},{},{},{}", addOrderMPID.getOrderReferenceNumber(), addOrderMPID.getBuySellIndicator(), addOrderMPID.getShares(), addOrderMPID.getStock(), addOrderMPID.getPrice(), addOrderMPID.getAttribution());
    }
    break;
    case MESSAGE_TYPE_TRADE_BROKEN:
    {
        return parseBrokenTradeOrOrderExecutionBody(std::move(header), data);
        // spdlog::get(CONSOLE_LOGGER) -> info("broken trade!");
    }
    break;
    case MESSAGE_TYPE_TRADE_CROSS:
    {
        return parseCrossTradeBody(std::move(header), data);
        // spdlog::get(CONSOLE_LOGGER) -> info("3. {},{},{},{},{}", crossTrade.getShares(), crossTrade.getStock(), crossTrade.getCrossPrice(), crossTrade.getMatchNumber(), crossTrade.getCrossType());
    }
    break;
    case MESSAGE_TYPE_ORDER_CANCELLED:
    {
        return parseOrderCancelBody(std::move(header), data);
        // spdlog::get(CONSOLE_LOGGER) -> info("4. {},{}",orderCancel.getOrderReferenceNumber(), orderCancel.getCancelledShares());
    }
    break;
    case MESSAGE_TYPE_ORDER_DELETE:
    {
        return parseOrderDeleteBody(std::move(header), data);
        // spdlog::get(CONSOLE_LOGGER) -> info("5. Deleting: {}", orderDelete.getOrderReferenceNumber());
    }
    break;
    case MESSAGE_TYPE_ORDER_EXECUTED:
    {
        return parseOrderExecutedBody(std::move(header), data);
        // spdlog::get(CONSOLE_LOGGER) -> info("6. {},{},{}", orderExecuted.getOrderReferenceNumber(), orderExecuted.getExecutedShares(), orderExecuted.getMatchNumber());
    }
    break;
    case MESSAGE_TYPE_ORDER_EXECUTED_WITH_PRICE:
    {
        return parseOrderExecutedWithPriceBody(std::move(header), data);
        // spdlog::get(CONSOLE_LOGGER) -> info("7. {},{},{},{},{}", orderExecutedWithPrice.getOrderReferenceNumber(), orderExecutedWithPrice.getExecutedShares(), orderExecutedWithPrice.geMtatchNumber(), orderExecutedWithPrice.getPrintable(), orderExecutedWithPrice.getExecutionPrice());
    }
    break;
    case MESSAGE_TYPE_ORDER_REPLACE:
    {
        return parseOrderReplaceBody(std::move(header), data);
        // spdlog::get(CONSOLE_LOGGER) -> info("8. {},{},{},{}", orderReplace.getOriginalOrderReferenceNumber(), orderReplace.getNewOrderReferenceNumber(), orderReplace.getShares(), orderReplace.getPrice());
    }
    break;
    case MESSAGE_TYPE_STOCK_TRADING_ACTION:
    {
        return parseStockTradingActionBody(std::move(header), data);
        // spdlog::get(CONSOLE_LOGGER) -> info("9. StockLocate {}: {},{},{},{}", header.getStockLocate(), stockTradingAction->stock, stockTradingAction->tradingState, stockTradingAction->reserved, stockTradingAction->reason);
    }
    break;
    case MESSAGE_TYPE_SYSTEM_EVENT:
    {
        return parseSystemEventBody(std::move(header), data);
        // spdlog::get(CONSOLE_LOGGER) -> info("{}",systemEvent -> eventCode);
    }
    break;
    case MESSAGE_TYPE_TRADE_NON_CROSS:
    {
        // spdlog::get(CONSOLE_LOGGER) -> info("10. {},{},{},{},{},{}", tradeNonCross.getOrderReferenceNumber(), tradeNonCross.getBuySellIndicator(), tradeNonCross.getShares(), tradeNonCross.geStock(), tradeNonCross.getPrice(), tradeNonCross.getMatchNumber());
        return parseTradeNonCrossBody(std::move(header), data);
    }
    break;

    default:
        return nullptr;
    }
}

/**
 * Given a message type, return how many bytes its body is
 */
size_t ProcessMessage::messageTypeToNumberOfBytes(char messageType)
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