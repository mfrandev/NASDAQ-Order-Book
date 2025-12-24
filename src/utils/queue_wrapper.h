

#ifndef QUEUE_WRAPPER_H_
#define QUEUE_WRAPPER_H_

#include <lockfree_queue.h>

#include <BinaryMessageWrapper.h>


struct alignas(SHARD_SIZE) queue_type {
    rigtorp::SPSCQueue<BinaryMessageWrapper> buffer;
};

#endif // QUEUE_WRAPPER_H_