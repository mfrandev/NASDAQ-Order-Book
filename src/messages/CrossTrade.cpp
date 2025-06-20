#include <CrossTrade.h>

#include <StockTradingAction.h>
#include <string_utils.h>
#include <endian_utils.h>

lockfree::MempoolSPSC<CrossTrade, SPSC_QUEUE_CAPACITY + 2> CrossTrade::_mempool;

/**
 * Parse the cross trade body from the binary buffer
 */
CrossTrade* parseCrossTradeBody(BinaryMessageHeader header, const char* data) {
    size_t offset = 0;
    uint64_t shares = toHostEndianUpTo64(&data[offset], 8); // We know this is an 8 byte int
    offset += 8;
    std::string stock = std::string(&data[offset], STOCK_SIZE);
    stripWhitespaceFromCPPString(stock);
    offset += STOCK_SIZE;
    uint32_t crossPrice = toHostEndianUpTo64(&data[offset], 4); // We know this is a 4 byte int
    offset += 4;
    uint64_t matchNumber = toHostEndianUpTo64(&data[offset], 8); // We know this is an 8 byte int
    offset += 8;
    char crossType = data[offset];
    return new CrossTrade(std::move(header), shares, stock, crossPrice, matchNumber, crossType);
}
