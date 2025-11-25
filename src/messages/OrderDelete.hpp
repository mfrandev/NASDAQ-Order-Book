#ifndef ORDER_DELETE_H_
#define ORDER_DELETE_H_

#include <endian_utils.hpp>
#include <MessageConstants.h>

struct OrderDelete {
    uint64_t orderReferenceNumber;
};

OrderDelete parseOrderDeleteBody(const char data[MAX_BUFFER_SIZE]) {
    size_t offset = 0;
    uint64_t orderReferenceNumber = toHostEndianUpTo64(&data[offset], MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE); // We know this is an 8 byte int
    return OrderDelete{orderReferenceNumber};
}

#endif // ORDER_DELETE_H_