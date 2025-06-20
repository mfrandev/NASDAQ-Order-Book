#ifndef NASDAQ_MESSAGES_ORDER_EXECUTED_H_
#define NASDAQ_MESSAGES_ORDER_EXECUTED_H_

#include <cstdint>

#include <Message.h>

#include <time_utils.h>
#include <VWAPManager.h>

#include <mempool.h>

// Store all other fields for the OrderExecuted message here
class OrderExecuted : public Message {

    private: 
        uint64_t orderReferenceNumber;
        uint32_t executedShares;
        uint64_t matchNumber;

        static lockfree::MempoolSPSC<OrderExecuted, SPSC_QUEUE_CAPACITY + 2> _mempool;

    public:
        OrderExecuted(BinaryMessageHeader header, uint64_t orderReferenceNumber, uint32_t executedShares, uint64_t matchNumber) :
        Message(std::move(header)),
        orderReferenceNumber(orderReferenceNumber),
        executedShares(executedShares),
        matchNumber(matchNumber)
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
            _mempool.deallocate(static_cast<OrderExecuted*>(ptr));
        }

        bool processMessage() const override { 
            if(isAfterHours(header.getTimestamp())) return true;
                VWAPManager::getInstance().handleOrderExecuted(
                header.getTimestamp(), 
                header.getStockLocate(), 
                orderReferenceNumber, 
                executedShares, 
                matchNumber
            );
            return true;
        } 

        void setOrderReferenceNumber(uint64_t orderReferenceNumber) { this -> orderReferenceNumber = orderReferenceNumber; }
        void setExecutedShares(uint32_t executedShares) { this -> executedShares = executedShares; }
        void setMatchNumber(uint64_t matchNumber) { this -> matchNumber = matchNumber; }
        
        uint64_t getOrderReferenceNumber() const { return this -> orderReferenceNumber; }
        uint32_t getExecutedShares() const { return this -> executedShares; }
        uint64_t getMatchNumber() const { return this -> matchNumber; }
};

OrderExecuted* parseOrderExecutedBody(BinaryMessageHeader header, const char* data);

#endif // NASDAQ_MESSAGES_ORDER_EXECUTED_H_