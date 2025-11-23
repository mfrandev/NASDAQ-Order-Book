#ifndef NASDAQ_MESSAGES_MESSAGE_HEADER_H_
#define NASDAQ_MESSAGES_MESSAGE_HEADER_H_

#include <MessageConstants.h>
#include <endian_utils.hpp>

// Format for the first 11 bytes of each message
struct MessageHeader {

        char messageType;
        uint16_t stockLocate;
        // uint16_t trackingNumber; // We never use this field
        uint64_t timestamp; 
    
};

/**
 * Take a pre-loaded sequence of bytes and parse the ITCH header out of it
 * Use the same portion of memory to store the header, just because
 */
MessageHeader parseHeader(const char* data) {
    size_t offset = 0;
    char messageType = data[offset];
    offset += MessageFieldSizes::MESSAGE_TYPE_SIZE;
    uint16_t stockLocate = toHostEndianUpTo64(&data[offset], MessageFieldSizes::STOCK_LOCATE_SIZE);
    offset += MessageFieldSizes::STOCK_LOCATE_SIZE;
    // We never use tracking number, so skip it 
    offset += MessageFieldSizes::TRACKING_NUMBER_SIZE;
    uint64_t timestamp = toHostEndianUpTo64(&data[offset], MessageFieldSizes::TIMESTAMP_SIZE);
    return MessageHeader{messageType, stockLocate, timestamp};
}

#endif // NASDAQ_MESSAGES_MESSAGE_HEADER_H_