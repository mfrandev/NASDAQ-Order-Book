#ifndef BROKEN_TRADE_OR_ORDER_EXECUTION_H_
#define BROKEN_TRADE_OR_ORDER_EXECUTION_H_

#include <MessageConstants.h>
#include <endian_utils.hpp>

struct BrokenTradeOrOrderExecution { 
    uint64_t matchNumber;
};

BrokenTradeOrOrderExecution parseBrokenTradeOrOrderExecutionBody(const char data[MAX_BUFFER_SIZE]) {
    size_t offset = 0;
    uint64_t matchNumber = toHostEndianUpTo64(&data[offset], MessageFieldSizes::MATCH_NUMBER_SIZE);
    return BrokenTradeOrOrderExecution{matchNumber};
}

#endif // BROKEN_TRADE_OR_ORDER_EXECUTION_H_