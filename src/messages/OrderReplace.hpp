#ifndef ORDER_REPLACE_H_
#define ORDER_REPLACE_H_

#include <endian_utils.hpp>
#include <MessageConstants.h>

struct OrderReplace {

    uint32_t shares;
    uint32_t price;
    uint64_t originalOrderReferenceNumber;
    uint64_t newOrderReferenceNumber;

};

OrderReplace parseOrderReplaceBody(const char* data) {
    size_t offset = 0;
    uint64_t originalOrderReferenceNumber = toHostEndianUpTo64(&data[offset], MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE); // We know this is an 8 byte int
    offset += MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE;
    uint64_t newOrderReferenceNumber = toHostEndianUpTo64(&data[offset], MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE); // We know this is an 8 byte int
    offset += MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE;
    uint32_t shares = toHostEndianUpTo64(&data[offset], MessageFieldSizes::SHARES_SIZE); // We know this is a 4 byte int
    offset += MessageFieldSizes::SHARES_SIZE;
    uint32_t price = toHostEndianUpTo64(&data[offset], MessageFieldSizes::PRICE_SIZE); // We know this is a 4 byte int
    return OrderReplace{shares, price, originalOrderReferenceNumber, newOrderReferenceNumber};
}


#endif // ORDER_REPLACE_H_