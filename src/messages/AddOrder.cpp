#include <AddOrder.h>

#include <StockTradingAction.h>
#include <string_utils.h>
#include <endian_utils.h>

#include <spdlog/spdlog.h>

lockfree::MempoolSPSC<AddOrder, SPSC_QUEUE_CAPACITY + 2> AddOrder::_mempool;
extern const std::string& CONSOLE_LOGGER;
/** 
 * Parse the AddOrder message body
 */
AddOrder* parseAddOrderBody(BinaryMessageHeader header, const char* data) {
    size_t offset = 0;
    uint64_t orderReferenceNumber = toHostEndianUpTo64(&data[offset], 8); // We know this is an 8 byte int
    offset += 8;
    char buySellIndicator = data[offset];
    offset += 1;
    uint32_t shares = toHostEndianUpTo64(&data[offset], 4); // We know this is a 4 byte int
    offset += 4;
    std::array<char, STOCK_SIZE> stock;
    // std::memcpy(&data[offset], stock, STOCK_SIZE);
    // stripWhitespaceFromCPPString(stock);
    // spdlog::get(CONSOLE_LOGGER) -> info("stock is {} bytes", sizeof(stock));
    offset += STOCK_SIZE;
    uint32_t price = toHostEndianUpTo64(&data[offset], 4); // We know this is a 4 byte int
    // AddOrder* t = new AddOrder(std::move(header), orderReferenceNumber, buySellIndicator, shares, std::move(stock), price);
    // spdlog::get(CONSOLE_LOGGER) -> info("add order is {} bytes", sizeof(*t));
    // return t;
    return new AddOrder(std::move(header), orderReferenceNumber, buySellIndicator, shares, std::move(stock), price);
}
