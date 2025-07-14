#include <StockTradingAction.h>

// #include <spdlog/spdlog.h>

#include <string_utils.h>

lockfree::MempoolSPSC<StockTradingAction, SPSC_QUEUE_CAPACITY + 2> StockTradingAction::_mempool;
// extern const std::string& CONSOLE_LOGGER;

/**
 * Parse the stock trading action body contents from the buffer
 */
StockTradingAction* parseStockTradingActionBody(BinaryMessageHeader header, const char* data) {
    size_t offset = 0;

    std::string stock = std::string(&data[offset], STOCK_SIZE);
    stripWhitespaceFromCPPString(stock);
    // spdlog::get(CONSOLE_LOGGER) -> info("stock is {} bytes", sizeof(stock));
    offset += STOCK_SIZE;

    char tradingState = data[offset];
    offset += TRADING_STATE_SIZE;

    char reserved = data[offset];
    offset += RESERVED_SIZE;

    std::string reason = std::string(&data[offset], REASON_SIZE);
    stripWhitespaceFromCPPString(reason);


    // StockTradingAction t = StockTradingAction(std::move(header), stock, tradingState, reserved, reason);
    // spdlog::get(CONSOLE_LOGGER) -> info("stock trading action is {} bytes", sizeof(t));
    // return &t;

    return new StockTradingAction(std::move(header), stock, tradingState, reserved, reason);
}