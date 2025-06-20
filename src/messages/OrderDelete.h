#ifndef NASDAQ_MESSAGES_ORDER_DELETE_H_
#define NASDAQ_MESSAGES_ORDER_DELETE_H_

#include <cstdint>

#include <Message.h>

#include <OrderBook.h>
#include <mempool.h>
#include <queue_utils.h>

// Class to store the Order Delete message body
class OrderDelete : public Message {

    private: 
        uint64_t orderReferenceNumber;

        static lockfree::MempoolSPSC<OrderDelete, SPSC_QUEUE_CAPACITY + 2> _mempool;
    
    public:
        OrderDelete(BinaryMessageHeader header, uint64_t orderReferenceNumber) :
        Message(std::move(header)),
        orderReferenceNumber(orderReferenceNumber)
        {}

        void* operator new(size_t) {
            void* ptr = _mempool.allocate();
            if (ptr == nullptr) throw std::bad_alloc();
            return ptr;
        }

        void operator delete(void* ptr) {
            _mempool.deallocate(static_cast<OrderDelete*>(ptr));
        }

        bool processMessage() const override { OrderBook::getInstance().deleteActiveOrder(orderReferenceNumber); return true; }

        void setOrderReferenceNumber(uint64_t orderReferenceNumber) { this -> orderReferenceNumber = orderReferenceNumber; }
        uint64_t getOrderReferenceNumber() const { return this -> orderReferenceNumber; }
};

// Parse the order delete message body
OrderDelete* parseOrderDeleteBody(BinaryMessageHeader header, const char* data);

#endif // NASDAQ_MESSAGES_ORDER_DELETE_H_