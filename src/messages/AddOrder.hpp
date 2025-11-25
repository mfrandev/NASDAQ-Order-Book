#ifndef ADD_ORDER_H_
#define ADD_ORDER_H_

#include <MessageConstants.h>
#include <endian_utils.hpp>

struct AddOrder {

    char buySellIndicator;
    // std::string stock; // We don't need to store this here
    uint32_t shares;
    uint32_t price;
    uint64_t orderReferenceNumber;

};

AddOrder parseAddOrderBody(const char data[MAX_BUFFER_SIZE]) {

    size_t offset = 0;
    uint64_t orderReferenceNumber = toHostEndianUpTo64(
        &data[offset], 
        MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE
    ); // We know this is an 8 byte int
    offset += MessageFieldSizes::ORDER_REFERENCE_NUMBER_SIZE;
    char buySellIndicator = data[offset];
    offset += MessageFieldSizes::BUY_SELL_INDICATOR_SIZE;
    uint32_t shares = toHostEndianUpTo64(&data[offset], MessageFieldSizes::SHARES_SIZE); // We know this is a 4 byte int
    offset += MessageFieldSizes::SHARES_SIZE;
    //std::array<char, STOCK_SIZE> stock; // We don't want to store this here
    offset += MessageFieldSizes::STOCK_SIZE;
    uint32_t price = toHostEndianUpTo64(&data[offset], MessageFieldSizes::PRICE_SIZE); // We know this is a 4 byte int
    return AddOrder{buySellIndicator, shares, price, orderReferenceNumber};
}

#endif // ADD_ORDER_H_