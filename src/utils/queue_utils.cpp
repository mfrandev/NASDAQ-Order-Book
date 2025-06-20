#include <queue_utils.h>

// Singleton declaration and getter of the boost lockfree SPSC queue wrapper
BoostLockfreeSPSC* BoostLockfreeSPSC::_instance = nullptr;
BoostLockfreeSPSC& BoostLockfreeSPSC::getInstance() {
    if(_instance == nullptr)
        _instance = new BoostLockfreeSPSC();
    return *_instance;
}

/**
 * Push an element to the queue
 */
bool BoostLockfreeSPSC::pushMesageToLockfreeSPSCQueue(Message* message) {
    return mq.push(message);
}

/**
 * Pop an element from the queue, make it accessible via the parameter passed to the function by reference
 */
bool BoostLockfreeSPSC::popMesageFromLockfreeSPSCQueue(Message*& message) {
    message = nullptr;
    return mq.pop(message);
}

// Singleton declaration and getter of the locking queue
template<typename T, const size_t QueueSize>
locking::QueueSPSC<T, QueueSize>* locking::QueueSPSC<T, QueueSize>::_instance = nullptr;

template<typename T, const size_t QueueSize>
locking::QueueSPSC<T, QueueSize>& locking::QueueSPSC<T, QueueSize>::getInstance() {
    if(_instance == nullptr) 
        _instance = new locking::QueueSPSC<T, QueueSize>();
    return *_instance;
}

template class locking::QueueSPSC<Message, SPSC_QUEUE_CAPACITY>;