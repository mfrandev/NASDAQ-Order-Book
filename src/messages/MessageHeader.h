#ifndef NASDAQ_MESSAGES_MESSAGE_HEADER_H_
#define NASDAQ_MESSAGES_MESSAGE_HEADER_H_

#include <cstdint>

// Format for the first 11 bytes of each message
class MessageHeader {

    private:
        char messageType;
        uint16_t stockLocate;
        // uint16_t trackingNumber;
        uint64_t timestamp; 
    
    public: 

        /**
         * Constructors and rule-of-5
         */
        MessageHeader(char messageType, uint16_t stockLocate, uint64_t timestamp) :
        messageType(messageType),
        stockLocate(stockLocate),
        timestamp(timestamp)
        {}
        MessageHeader() :
        messageType('0'),
        stockLocate(0),
        timestamp(0)
        {}
        ~MessageHeader() = default;
        MessageHeader(const MessageHeader&) = default;
        MessageHeader& operator=(const MessageHeader&) = default;
        MessageHeader(MessageHeader&& header) noexcept = default;
        MessageHeader& operator=(MessageHeader&& header) noexcept = default;

        void setMessageType(char messageType) { this -> messageType = messageType; }
        void setStockLocate(uint16_t stockLocate) { this -> stockLocate = stockLocate; }
        void setTimestamp(uint64_t timestamp) { this -> timestamp = timestamp; }

        char getMessageType() const { return this -> messageType; }
        uint16_t getStockLocate() const { return this -> stockLocate; }
        uint64_t getTimestamp() const { return this -> timestamp; }
};

constexpr int NUMBER_OF_BYTES_FOR_HEADER_CHUNK          = 13;
constexpr int NUMBER_OF_BYTES_OFFSET_FOR_HEADER_CHUNK   = 2;

MessageHeader parseHeader(const char* data);

#endif // NASDAQ_MESSAGES_MESSAGE_HEADER_H_