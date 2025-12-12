#ifndef BINARY_MESSAGE_WRAPPER_H_
#define BINARY_MESSAGE_WRAPPER_H_

#include <MessageHeader.hpp>

struct alignas(64) BinaryMessageWrapper {

    uint64_t seq;
    MessageHeader header;
    char buffer[MAX_BUFFER_SIZE]; // Later investigate

};

static_assert(sizeof(BinaryMessageWrapper) == 64);

#endif // BINARY_MESSAGE_WRAPPER_H_