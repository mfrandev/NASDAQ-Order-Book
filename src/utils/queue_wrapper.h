

#ifndef QUEUE_WRAPPER_H_
#define QUEUE_WRAPPER_H_

#include <readerwritercircularbuffer.h>
#include <lockfree_queue.h>

#include <BinaryMessageWrapper.h>

// using queue_type = moodycamel::BlockingReaderWriterCircularBuffer<BinaryMessageWrapper>;


struct alignas(SHARD_SIZE) queue_type {
    rigtorp::SPSCQueue<BinaryMessageWrapper> buffer;
};

#endif // QUEUE_WRAPPER_H_