#ifndef NASDAQ_MESSAGES_ORDER_EXECUTED_WITH_PRICE_H_
#define NASDAQ_MESSAGES_ORDER_EXECUTED_WITH_PRICE_H_

#include <cstdint>
#include <stdlib.h>

#include <Message.h>

#include <time_utils.h>
#include <VWAPManager.h>

#include <mempool.h>

// Store all other fields for the OrderExecutedWithPrice message here
struct OrderExecutedWithPrice : public Message {

    private:
        uint64_t orderReferenceNumber;
        uint32_t executedShares;
        uint64_t matchNumber;
        char printable;
        uint32_t executionPrice;

        static lockfree::MempoolSPSC<OrderExecutedWithPrice, SPSC_QUEUE_CAPACITY + 2> _mempool;

    public:
        OrderExecutedWithPrice(BinaryMessageHeader header, uint64_t orderReferenceNumber, uint32_t executedShares, uint64_t matchNumber, char printable, uint32_t executionPrice) :
        Message(std::move(header)),
        orderReferenceNumber(orderReferenceNumber),
        executedShares(executedShares),
        matchNumber(matchNumber),
        printable(printable),
        executionPrice(executionPrice)
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
            _mempool.deallocate(static_cast<OrderExecutedWithPrice*>(ptr));
        }

        bool processMessage() const override { 
            if(isAfterHours(header.getTimestamp())) return true;            
            VWAPManager::getInstance().handleOrderExecutedWithPrice(
                header.getTimestamp(), 
                header.getStockLocate(), 
                orderReferenceNumber,
                executedShares,
                matchNumber,
                printable,
                executionPrice
            );
            return true;
        } 

        void getOrderReferenceNumber(uint64_t orderReferenceNumber) { this -> orderReferenceNumber = orderReferenceNumber; }
        void getExecutedShares(uint32_t executedShares) { this -> executedShares = executedShares; }
        void getMatchNumber(uint64_t matchNumber) { this -> matchNumber = matchNumber; }
        void getPrintable(char printable) { this -> printable = printable; }
        void getExecutionPrice(uint32_t executionPrice) { this -> executionPrice = executionPrice; }

        uint64_t getOrderReferenceNumber() const { return this -> orderReferenceNumber; }
        uint32_t getExecutedShares() const { return this -> executedShares; }
        uint64_t getMatchNumber() const { return this -> matchNumber; }
        char getPrintable() const { return this -> printable; }
        uint32_t getExecutionPrice() const { return this -> executionPrice; }
};

OrderExecutedWithPrice* parseOrderExecutedWithPriceBody(BinaryMessageHeader header, const char* data);

#endif // NASDAQ_MESSAGES_ORDER_EXECUTED_WITH_PRICE_H_