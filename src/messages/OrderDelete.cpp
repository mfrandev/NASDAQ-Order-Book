#include <OrderDelete.h>

#include <endian_utils.h>

lockfree::MempoolSPSC<OrderDelete, SPSC_QUEUE_CAPACITY + 2> OrderDelete::_mempool;

/**
 * Parse the OrderDelete body
 */
OrderDelete* parseOrderDeleteBody(BinaryMessageHeader header, const char* data) {
    size_t offset = 0;
    uint64_t orderReferenceNumber = toHostEndianUpTo64(&data[offset], 8); // We know this is an 8 byte int
    return new OrderDelete(std::move(header), orderReferenceNumber);
}