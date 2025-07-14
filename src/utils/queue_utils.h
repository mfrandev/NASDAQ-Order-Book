#ifndef NASDAQ_UTILS_QUEUE_UTILS_H_
#define NASDAQ_UTILS_QUEUE_UTILS_H_

#include <memory>
#include <mutex>
#include <condition_variable>

#include <Message.h>
#include <lockfree_queue.h>

#include <boost/lockfree/spsc_queue.hpp>

// Important to define, since this will the maximum memory pool size for high freq message pools in SPSC implementation
constexpr size_t SPSC_QUEUE_CAPACITY = 1024;

/**
 * A little wrapper for the boost lockfree SPSC queue
 */
class BoostLockfreeSPSC {

    public:
        static BoostLockfreeSPSC& getInstance();

        bool pushMesageToLockfreeSPSCQueue(Message* message);
        bool popMesageFromLockfreeSPSCQueue(Message*& message);
        size_t write_available() { return mq.write_available(); }
        size_t read_available() { return mq.read_available(); }

    private:
        static BoostLockfreeSPSC* _instance;
        boost::lockfree::spsc_queue<Message*, boost::lockfree::capacity<SPSC_QUEUE_CAPACITY>> mq;
};

/**
 * Mutex/Condition Variable Locking Queue
 */
namespace locking {

    template <typename T, const size_t QueueSize>
    class QueueSPSC {  

        private:
            // For synchronization
            std::mutex _m;
            std::condition_variable _cv;

            size_t front;
            size_t back;

            alignas(T) std::byte buffer[sizeof(T) * QueueSize];
            T* isFreeBufferMap[QueueSize];

            static QueueSPSC<T, QueueSize>* _instance;

        public:
            static QueueSPSC<T, QueueSize>& getInstance();
            QueueSPSC() {
                for(int i = 0; i < QueueSize; i++) 
                    isFreeBufferMap[i] = reinterpret_cast<T*>(buffer + (i * sizeof(T)));
                front = 0;
                back = 0;
            }

            bool threadsafe_spsc_is_empty() {
                std::scoped_lock lk(_m);
                return front == back;
            }

            /**
             * If front equals back, then queue is empty
             */
            T* threadsafe_spsc_pop() {
                std::unique_lock<std::mutex> lk(_m);
                _cv.wait(lk, [this] {
                    return (this -> front != this -> back);
                });
                T* elem = isFreeBufferMap[back];
                auto next = back + 1;
                if(next == QueueSize) next = 0;
                back = next;
                _cv.notify_one();
                return elem;
            }

            /**
             * If front of queue + 1 is equal to the back, the queue is full.
             */
            void threadsafe_spsc_push(T* elem) {
                std::unique_lock<std::mutex> lk(_m);
                _cv.wait(lk, [this] { 
                    auto next = this -> front + 1;
                    if(next == QueueSize) next = 0;
                    return next != this -> back;
                });
                auto next = front + 1;
                if(next == QueueSize) next = 0;
                isFreeBufferMap[front] = elem;
                front = next;
                _cv.notify_one();
            }
    };
}

#endif // NASDAQ_UTILS_QUEUE_UTILS_H_