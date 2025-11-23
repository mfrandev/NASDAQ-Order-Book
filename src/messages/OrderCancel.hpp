#ifndef ORDER_CANCEL_H_
#define ORDER_CANCEL_H_

#include <endian_utils.hpp>
#include <MessageConstants.h>

struct OrderCancel {

    uint32_t cancelledShares;
    uint64_t orderReferenceNumber;

};

OrderCancel parseOrderCancelBody(const char* data) {
    size_t offset = 0;
    uint64_t orderReferenceNumber = toHostEndianUpTo64(&data[offset], MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE); // We know this is an 8 byte int
    offset += MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE;
    uint32_t cancelledShares = toHostEndianUpTo64(&data[offset], MessageFieldSizes::CANCELLED_SHARES_SIZE); // We know this is a 4 byte int
    return OrderCancel{cancelledShares, orderReferenceNumber};
}

#endif // ORDER_CANCEL_H_