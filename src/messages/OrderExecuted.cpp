#include <OrderExecuted.h>

#include <endian_utils.h>

lockfree::MempoolSPSC<OrderExecuted, SPSC_QUEUE_CAPACITY + 2> OrderExecuted::_mempool;

OrderExecuted* parseOrderExecutedBody(BinaryMessageHeader header, const char* data) {
    size_t offset = 0;
    uint64_t orderReferenceNumber = toHostEndianUpTo64(&data[offset], 8); // We know this is 8 bytes
    offset += 8;
    uint32_t executedShares = toHostEndianUpTo64(&data[offset], 4); // We know this is 4 bytes
    offset += 4;
    uint64_t matchNumber = toHostEndianUpTo64(&data[offset], 8); // We know this is 8 bytes
    return new OrderExecuted(std::move(header), orderReferenceNumber, executedShares, matchNumber);
}
