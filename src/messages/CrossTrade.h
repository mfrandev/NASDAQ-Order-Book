#ifndef NASDAQ_MESSAGES_CROSS_TRADE_H_
#define NASDAQ_MESSAGES_CROSS_TRADE_H_

#include <cstdint>
#include <string>

#include <Message.h>

#include <time_utils.h>
#include <VWAPManager.h>

#include <mempool.h>

// Class for parsing the CrossTrade binary message
class CrossTrade : public Message {
    private:
        uint64_t shares;
        std::string stock;
        uint32_t crossPrice;
        uint64_t matchNumber;
        char crossType;

        static lockfree::MempoolSPSC<CrossTrade, SPSC_QUEUE_CAPACITY + 2> _mempool;

    public:
        /**
         * Rule of 5 compliant
         */
        CrossTrade(BinaryMessageHeader header, uint64_t shares, std::string stock, uint32_t crossPrice, uint64_t matchNumber, char crossType) :
        Message(std::move(header)),
        shares(shares),
        stock(std::move(stock)),
        crossPrice(crossPrice),
        matchNumber(matchNumber),
        crossType(crossType) 
        {}

        bool processMessage() const override {
            if(isAfterHours(header.getTimestamp())) return true;
            VWAPManager::getInstance().handleCrossTrade(header.getTimestamp(), header.getStockLocate(), crossPrice, shares, matchNumber);
            return true;
        }

        /**
         * Overload new/delete to use the mempool, rather than heap allocations
         */
        void* operator new(size_t) {
            void* ptr = _mempool.allocate();
            if (ptr == nullptr) throw std::bad_alloc();
            return ptr;
        }

        void operator delete(void* ptr) {
            _mempool.deallocate(static_cast<CrossTrade*>(ptr));
        }

        void setShares(uint64_t shares) { this -> shares = shares; }
        void setStock(std::string stock) { this -> stock = std::move(stock); }
        void setCrossPrice(uint32_t crossPrice) { this -> crossPrice = crossPrice; }
        void setMatchNumber(uint64_t matchNumber) { this -> matchNumber = matchNumber; }
        void setCrossType(char crossType) { this -> crossType = crossType; }

        uint64_t getShares() const { return this -> shares; }
        std::string getStock() const { return this -> stock; }
        uint32_t getCrossPrice() const { return this -> crossPrice; }
        uint64_t getMatchNumber() const { return this -> matchNumber; }
        char getCrossType() const { return this -> crossType; }
};

CrossTrade* parseCrossTradeBody(BinaryMessageHeader header, const char* data);

#endif // NASDAQ_MESSAGES_CROSS_TRADE_H_