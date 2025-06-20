#include <BrokenTradeOrOrderExecution.h>

#include <endian_utils.h>

lockfree::MempoolSPSC<BrokenTradeOrOrderExecution, SPSC_QUEUE_CAPACITY + 2> BrokenTradeOrOrderExecution::_mempool;

/**
 * Parse the BrokenTradeOrOrderExecution body
 */
BrokenTradeOrOrderExecution* parseBrokenTradeOrOrderExecutionBody(BinaryMessageHeader header, const char* data) {
    size_t offset = 0;
    uint64_t matchNumber = toHostEndianUpTo64(&data[offset], 8); // We know this is an 8 byte int
    return new BrokenTradeOrOrderExecution(std::move(header), matchNumber);
}
