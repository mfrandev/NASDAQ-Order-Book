#ifndef NASDAQ_MESSAGE_H_
#define NASDAQ_MESSAGE_H_

/**
 * Parent class for all ITCH messages
 */

#include <utility> // for std::move

#include <MessageHeader.h>

class Message {

    protected:
        BinaryMessageHeader header;

    public:
        explicit Message(BinaryMessageHeader header) : 
        header(std::move(header))
        {}
        explicit Message() = default;
        virtual ~Message() = default;

        Message(const Message&) = default;
        Message& operator=(const Message&) = default;
        Message(Message&& header) noexcept = default;
        Message& operator=(Message&& header) noexcept = default;

        virtual bool processMessage() const = 0; 

        const BinaryMessageHeader& getHeader() const { return header; }
        void setHeader(BinaryMessageHeader header) { this -> header = std::move(header); }
};

#endif // NASDAQ_MESSAGE_H_