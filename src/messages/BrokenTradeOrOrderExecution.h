#ifndef NASDAQ_MESSAGES_BROKEN_TRADE_OR_ORDER_EXECUTION_H_
#define NASDAQ_MESSAGES_BROKEN_TRADE_OR_ORDER_EXECUTION_H_

#include <cstdint>

#include <Message.h>

#include <VWAPManager.h>

#include <mempool.h>

// Class to store the Broken Trade or Order Execution message body
class BrokenTradeOrOrderExecution : public Message {
    private:
        uint64_t matchNumber;
        static lockfree::MempoolSPSC<BrokenTradeOrOrderExecution, SPSC_QUEUE_CAPACITY + 2> _mempool;
    public:
        BrokenTradeOrOrderExecution(BinaryMessageHeader header, uint64_t matchNumber) :
        Message(std::move(header)),
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
            _mempool.deallocate(static_cast<BrokenTradeOrOrderExecution*>(ptr));
        }

        bool processMessage() const override { VWAPManager::getInstance().handleBrokenTrade(header.getStockLocate(), matchNumber); return true; }

        void setMatchNumber(uint64_t matchNumber) { this -> matchNumber = matchNumber; }
        uint64_t getMatchNumber() const { return this -> matchNumber; } 
};

// Parse the Broken Trade or Order Execution message body
BrokenTradeOrOrderExecution* parseBrokenTradeOrOrderExecutionBody(BinaryMessageHeader header, const char* data);

#endif // NASDAQ_MESSAGES_BROKEN_TRADE_OR_ORDER_EXECUTION_H_