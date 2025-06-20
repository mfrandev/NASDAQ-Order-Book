#ifndef NASDAQ_MESSAGES_STOCK_TRADING_ACTION_H_
#define NASDAQ_MESSAGES_STOCK_TRADING_ACTION_H_

#include <string>
#include <cstdint>

#include <Message.h>

#include <VWAPManager.h>

#include <mempool.h>

// Trade state reason constants
constexpr char TRADING_STATE_REASON_HALTED            = 'H';
constexpr char TRADING_STATE_REASON_PAUSED            = 'P';
constexpr char TRADING_STATE_REASON_QUOTATION         = 'Q';
constexpr char TRADING_STATE_REASON_TRADING_ON_NASDAQ = 'T';

// Stock trading action message body
constexpr size_t STOCK_SIZE         = 8;
constexpr size_t TRADING_STATE_SIZE = 1;
constexpr size_t RESERVED_SIZE      = 1;
constexpr size_t REASON_SIZE        = 4;

class StockTradingAction : public Message {
    private:
        std::string stock;
        char tradingState;
        char reserved;
        std::string reason;

        static lockfree::MempoolSPSC<StockTradingAction, SPSC_QUEUE_CAPACITY + 2> _mempool;

    public:

        /**
         * Rule-of-5 implcitly implemented, since char is trivial and std::string is implemented to be compliant as well
         */
        StockTradingAction(BinaryMessageHeader header, std::string stock, char tradingState, char reserved, std::string reason) :
        Message(std::move(header)),
        stock(std::move(stock)),
        tradingState(tradingState),
        reserved(reserved),
        reason(std::move(reason))
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
            _mempool.deallocate(static_cast<StockTradingAction*>(ptr));
        }

        bool processMessage() const override { VWAPManager::getInstance().handleStockTradingAction(header.getStockLocate(), stock, tradingState); return true; }

        void setStock(std::string stock) { this -> stock = std::move(stock); }
        void setTradingState(char tradingState) { this -> tradingState = tradingState; }
        void setReserved(char reserved) { this -> reserved = reserved; }
        void setReason(std::string reason) { this -> reason = std::move(reason); }

        std::string getStock() const { return this -> stock; }
        char getTradingState() const { return this -> tradingState; }
        char getReserved() const {  return this -> reserved; }
        std::string getReason() const { return this -> reason; }
};

StockTradingAction* parseStockTradingActionBody(BinaryMessageHeader header, const char* data);

#endif // NASDAQ_MESSAGES_STOCK_TRADING_ACTION_H_