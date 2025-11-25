#ifndef ADD_ORDER_WITH_MPID_H_
#define ADD_ORDER_WITH_MPID_H_

#include <endian_utils.hpp>
#include <MessageConstants.h>

struct AddOrderWithMPID {

    char buySellIndicator;
    uint32_t shares;
    uint32_t price;
    uint64_t orderReferenceNumber;
    // std::string stock;
    // std::string attribution;

};

AddOrderWithMPID parseAddOrderWithMPIDBody(const char data[MAX_BUFFER_SIZE]) {
    size_t offset = 0;
    uint64_t orderReferenceNumber = toHostEndianUpTo64(&data[offset], MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE); // We know this is an 8 byte int
    offset += MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE;
    char buySellIndicator = data[offset];
    offset += MessageFieldSizes::BUY_SELL_INDICATOR_SIZE;
    uint32_t shares = toHostEndianUpTo64(&data[offset], MessageFieldSizes::SHARES_SIZE); // We know this is a 4 byte int
    offset += MessageFieldSizes::SHARES_SIZE;
    // std::string stock;
    offset += MessageFieldSizes::STOCK_SIZE;
    uint32_t price = toHostEndianUpTo64(&data[offset], MessageFieldSizes::PRICE_SIZE); // We know this is a 4 byte int
    offset += MessageFieldSizes::PRICE_SIZE;
    // std::string attribution;
    return AddOrderWithMPID{buySellIndicator, shares, price, orderReferenceNumber};
}

#endif // ADD_ORDER_WITH_MPID_H_