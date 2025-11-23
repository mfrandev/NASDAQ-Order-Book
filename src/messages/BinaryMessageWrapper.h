#ifndef BINARY_MESSAGE_WRAPPER_H_
#define BINARY_MESSAGE_WRAPPER_H_

#include <MessageHeader.hpp>

struct alignas(64) BinaryMessageWrapper {

    MessageHeader header;
    char buffer[MAX_BUFFER_SIZE]; // Later investigate

};

#endif // BINARY_MESSAGE_WRAPPER_H_