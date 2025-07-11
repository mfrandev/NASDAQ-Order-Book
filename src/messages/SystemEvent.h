#ifndef NASDAQ_MESSAGES_SYSTEM_EVENT_H_
#define NASDAQ_MESSAGES_SYSTEM_EVENT_H_

#include <cstdint>
#include <stdlib.h>

#include <Message.h>

// System Event Codes
constexpr char EVENT_CODE_START_OF_MESSAGES     = 'O';
constexpr char EVENT_CODE_START_OF_SYSTEM_HOURS = 'S';
constexpr char EVENT_CODE_START_OF_MARKET_HOURS = 'Q';
constexpr char EVENT_CODE_END_OF_MARKET_HOURS   = 'M';
constexpr char EVENT_CODE_END_OF_SYSTEM_HOURS   = 'E';
constexpr char EVENT_CODE_END_OF_MESSAGES       = 'C';

constexpr size_t EVENT_CODE_SIZE = 1;

// Class for storing the System Event message body
class SystemEvent : public Message {

    private:
        char eventCode;

    public:

        /**
         * Compiler will generate the default destuctor copy/move operator= and copy/move constructors
         */
        SystemEvent(BinaryMessageHeader header, char eventCode) : 
        Message(std::move(header)),
        eventCode(eventCode) 
        {}

        bool processMessage() const override; 

        void setEventCode(char eventCode) { this -> eventCode = eventCode; }
        char getEventCode() const { return this -> eventCode; } 
};

SystemEvent* parseSystemEventBody(BinaryMessageHeader header, const char* data);

#endif // NASDAQ_MESSAGES_SYSTEM_EVENT_H_