#ifndef SHARD_CONSUMER_H_
#define SHARD_CONSUMER_H_

#include <immintrin.h> // For _mm_pause
#include <thread>
#include <condition_variable>
#include <memory>
#include <atomic>

#include <ThreadConstants.h>
#include <thread_utils.hpp>
#include <queue_wrapper.h>
#include <Message.hpp>

#include <iostream>
// Global counter tracking how many messages consumers have processed
// inline std::atomic<uint64_t> numMessages{0};

class ShardConsumer {

    private:

        void backoff(queue_type& queue) {
            // Step 1: Busy wait
            for(auto i = 0; i < ThreadConstants::BUSY_WAIT_CYCLES; ++i) {
                _mm_pause();
                if(!queue.buffer.empty()) return;
            }
            
            // Step 2: thread yield
            for(auto i = 0; i < ThreadConstants::YIELD_CYCLES; ++i) {
                std::this_thread::yield();
                if(!queue.buffer.empty()) return;
            }

            // Step 3: Condvar sleep
            // _cv.wait();
        }

        void processMessage(Message message) {
            
        }

    public:

        std::atomic<bool> finished{false};

        void run(queue_type& queue, int run_on_this_core) {

            std::cout << run_on_this_core << std::endl;
    
            if(!pin_to_core(run_on_this_core)) {
                std::cout << "Failed to pin consumer " << (((run_on_this_core-ThreadConstants::FIRST_CONSUMER_CORE)/2)+1) << " to core " << run_on_this_core << std::endl;
            }
            while(!finished.load()) {
                if(!queue.buffer.empty()) {
                    BinaryMessageWrapper bmsg = *(queue.buffer.front());
                    queue.buffer.pop();
                    // numMessages.fetch_add(1, std::memory_order_relaxed);
                    MessageBody msgBody = parseMessage(bmsg.header.messageType, bmsg.header.stockLocate, bmsg.buffer);
                    processMessage(Message{bmsg.header, msgBody});
                } else {
                    std::this_thread::yield();
                }
            } 
        }
    
};

#endif  // SHARD_CONSUMER_H_
