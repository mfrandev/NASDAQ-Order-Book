#ifndef ORDER_EXECUTED_H_
#define ORDER_EXECUTED_H_

#include <endian_utils.hpp>
#include <MessageConstants.h>

struct OrderExecuted {
    
    uint32_t executedShares;
    uint64_t matchNumber;
    uint64_t orderReferenceNumber;
    
};

OrderExecuted parseOrderExecutedBody(const char* data) {
    size_t offset = 0;
    uint64_t orderReferenceNumber = toHostEndianUpTo64(&data[offset], MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE); // We know this is 8 bytes
    offset += MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE;
    uint32_t executedShares = toHostEndianUpTo64(&data[offset], MessageFieldSizes::EXECUTED_SHARES_SIZE); // We know this is 4 bytes
    offset += MessageFieldSizes::EXECUTED_SHARES_SIZE;
    uint64_t matchNumber = toHostEndianUpTo64(&data[offset], MessageFieldSizes::MATCH_NUMBER_SIZE); // We know this is 8 bytes
    return OrderExecuted{executedShares, matchNumber, orderReferenceNumber};
}

#endif // ORDER_EXECUTED_H_