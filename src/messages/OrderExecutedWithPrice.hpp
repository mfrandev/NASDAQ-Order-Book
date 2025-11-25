#ifndef ORDER_EXECUTED_WITH_PRICE_H_
#define ORDER_EXECUTED_WITH_PRICE_H_

#include <endian_utils.hpp>
#include <MessageConstants.h>

struct OrderExecutedWithPrice {

    char printable; // This may be able to get removed
    uint32_t executedShares;
    uint32_t executionPrice;
    uint64_t orderReferenceNumber;
    uint64_t matchNumber;

};

OrderExecutedWithPrice parseOrderExecutedWithPriceBody(const char data[MAX_BUFFER_SIZE]) {
    size_t offset = 0;
    uint64_t orderReferenceNumber = toHostEndianUpTo64(&data[offset], MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE); // We know this is 8 bytes
    offset += MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE;
    uint32_t executedShares = toHostEndianUpTo64(&data[offset], MessageFieldSizes::EXECUTED_SHARES_SIZE); // We know this is 4 bytes
    offset += MessageFieldSizes::EXECUTED_SHARES_SIZE;
    uint64_t matchNumber = toHostEndianUpTo64(&data[offset], MessageFieldSizes::MATCH_NUMBER_SIZE); // We know this is 8 bytes
    offset += MessageFieldSizes::MATCH_NUMBER_SIZE;
    char printable = data[offset];
    offset += MessageFieldSizes::PRINTABLE_SIZE;
    uint32_t executionPrice = toHostEndianUpTo64(&data[offset], MessageFieldSizes::EXECUTION_PRICE_SIZE); // We know this is 4 bytes
    return OrderExecutedWithPrice{printable, executedShares, executionPrice, orderReferenceNumber, matchNumber};
}


#endif // ORDER_EXECUTED_WITH_PRICE_H_