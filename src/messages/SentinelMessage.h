#ifndef NASDAQ_SENTINEL_MESSAGE_H_
#define NASDAQ_SENTINEL_MESSAGE_H_

#include <Message.h>

class SentinelMessage : public Message {

    public:
        SentinelMessage() : Message() {};
        bool processMessage() const override { return false; }

};

#endif // NASDAQ_SENTINEL_MESSAGE_H_