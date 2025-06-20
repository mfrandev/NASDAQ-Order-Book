#ifndef NASDAQ_DATA_STRUCTURES_MEMPOOL_H_
#define NASDAQ_DATA_STRUCTURES_MEMPOOL_H_

#include <cmath>
#include <cassert>
#include <atomic>
#include <queue_utils.h>

namespace lockfree {

    /**
     * Memory Pool for static allocation of frequently produced messages in the SPSC model.
     * To avoid needing mutex locks, hold invariant that MempoolSize must not exceed the size of the queue.
     * See https://rigtorp.se/ringbuffer/ for lockfree ring buffer implementation reference.
     */
    template <typename T, const size_t MempoolSize>
    class MempoolSPSC {

        /**
         * Whole point of the SPSC mempool is to NEVER return a nullptr when requesting a resource.
         * It is possible that:
         * 1. The entire SPSC queue may be full of a single message instance (empty mempool).
         * 2. The consumer may pop an instance off of the queue for processing (empty mempool, but queue is ready for writes).
         * 3. The producer requests another instance of the same message type to add to the queue BEFORE THE CONSUMER INVOKES DELETE, returning the resource to the mempool.
         * Thus, we must account for the fact that there may be as many as SPSC_QUEUE_CAPACITY + 1 messages of a single instance in existance at any given point. 
         */
        static_assert(MempoolSize > SPSC_QUEUE_CAPACITY + 1);

        private:
            alignas(T) std::byte buffer[sizeof(T) * MempoolSize];
            T* isFreeBufferMap[MempoolSize];

            // On AMD64/x86_64 and ARM CPUs, cache lines are 64 bytes
            alignas(64) std::atomic<unsigned long> _allocatorIndex{0};
            alignas(64) std::atomic<unsigned long> _deallocatorIndex{0};

            // If 2, then this type is always lock free, not determined at runtime
            static_assert(ATOMIC_LONG_LOCK_FREE == 2);

        public:
            MempoolSPSC() {
                for(size_t i = 0; i < MempoolSize; i++)
                    isFreeBufferMap[i] = reinterpret_cast<T*>(buffer + (i * sizeof(T)));
            }

            /**
             * Return a pointer to a readily available resource
             */
            T* allocate() {
                auto const allocatorIndex = _allocatorIndex.load(std::memory_order_relaxed);
                auto nextAllocatorIndex = allocatorIndex + 1;
                if(nextAllocatorIndex == MempoolSize) 
                    nextAllocatorIndex = 0;
                if(nextAllocatorIndex == _deallocatorIndex.load(std::memory_order_acquire))
                    return nullptr;
                T* resource = isFreeBufferMap[allocatorIndex];
                _allocatorIndex.store(nextAllocatorIndex, std::memory_order_release);
                return resource;
            }

            /**
             * Take an allocated resource and return it to the memory pool
             */
            void deallocate(T* item) {
                auto const deallocatorIndex = _deallocatorIndex.load(std::memory_order_relaxed);
                if(deallocatorIndex == _allocatorIndex.load(std::memory_order_acquire))
                    return;
                isFreeBufferMap[deallocatorIndex] = item;
                auto nextDeallocatorIndex = deallocatorIndex + 1;
                if(nextDeallocatorIndex == MempoolSize)
                    nextDeallocatorIndex = 0;
                _deallocatorIndex.store(nextDeallocatorIndex, std::memory_order_release);
            }
    };
}

#endif // NASDAQ_DATA_STRUCTURES_MEMPOOL_H_