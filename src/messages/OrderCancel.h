#ifndef NASDAQ_MESSAGES_ORDER_CANCEL_H_
#define NASDAQ_MESSAGES_ORDER_CANCEL_H_

#include <cstdint>

#include <Message.h>
#include <OrderBook.h>

#include <mempool.h>

// Class to store the Order Cancel message body
class OrderCancel : public Message {
    private: 
        uint64_t orderReferenceNumber;
        uint32_t cancelledShares;

        static lockfree::MempoolSPSC<OrderCancel, SPSC_QUEUE_CAPACITY + 2> _mempool;

    public: 
        OrderCancel(BinaryMessageHeader header, uint64_t orderReferenceNumber, uint32_t cancelledShares) :
        Message(std::move(header)),
        orderReferenceNumber(orderReferenceNumber),
        cancelledShares(cancelledShares)
        {}

        /**
         * Overload new/delete to use the mempool, rather than heap allocations
         */
        void* operator new(size_t) {
            void* ptr = _mempool.allocate();
            if (ptr == nullptr) throw std::bad_alloc();
            return ptr;
        }

        void operator delete(void* ptr) {
            _mempool.deallocate(static_cast<OrderCancel*>(ptr));
        }

        bool processMessage() const override { OrderBook::getInstance().cancelActiveOrder(orderReferenceNumber, cancelledShares); return true; } 

        void setOrderReferenceNumber(uint64_t orderReferenceNumber) { this -> orderReferenceNumber = orderReferenceNumber; }
        void setCancelledShares(uint32_t cancelledShares) { this -> cancelledShares = cancelledShares; }

        uint64_t getOrderReferenceNumber() const { return this -> orderReferenceNumber; }
        uint32_t getCancelledShares() const { return this -> cancelledShares; }
};

// Parse the order cancel message body
OrderCancel* parseOrderCancelBody(BinaryMessageHeader header, const char* data);

#endif // NASDAQ_MESSAGES_ORDER_CANCEL_H_