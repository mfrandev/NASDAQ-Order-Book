#include <OrderCancel.h>

#include <endian_utils.h>

lockfree::MempoolSPSC<OrderCancel, SPSC_QUEUE_CAPACITY + 2> OrderCancel::_mempool;

/**
 * Parse the OrderCancel body
 */
OrderCancel* parseOrderCancelBody(BinaryMessageHeader header, const char* data) {
    size_t offset = 0;
    uint64_t orderReferenceNumber = toHostEndianUpTo64(&data[offset], 8); // We know this is an 8 byte int
    offset += 8;
    uint32_t cancelledShares = toHostEndianUpTo64(&data[offset], 4); // We know this is a 4 byte int
    return new OrderCancel(std::move(header), orderReferenceNumber, cancelledShares);
}