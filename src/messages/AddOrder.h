#ifndef NASDAQ_MESSAGES_ADD_ORDER_H_
#define NASDAQ_MESSAGES_ADD_ORDER_H_

#include <string>
#include <cstdint>

#include <Message.h>
#include <MessageHeader.h>

#include <OrderBook.h>

#include <mempool.h>

constexpr size_t BUY_SELL_INDICATOR_SIZE = 1;
// constexpr size_t STOCK_SIZE = 8; Use the value from StockTradingAction.h

// Store the Add Order message body
class AddOrder : public Message {
    private:
        uint64_t orderReferenceNumber;
        char buySellIndicator;
        uint32_t shares;
        std::string stock;
        uint32_t price;

        static lockfree::MempoolSPSC<AddOrder, SPSC_QUEUE_CAPACITY + 2> _mempool;
    
    public:
        /**
         * Rule of 5 compliant
         */
        AddOrder(BinaryMessageHeader header, uint64_t orderReferenceNumber, char buySellIndicator, uint32_t shares, std::string stock, uint32_t price) :
        Message(std::move(header)),
        orderReferenceNumber(orderReferenceNumber),
        buySellIndicator(buySellIndicator),
        shares(shares),
        stock(std::move(stock)),
        price(price)
        {}

        ~AddOrder() = default;
        AddOrder(const AddOrder&) = default;
        AddOrder(AddOrder&&) noexcept = default;
        AddOrder& operator=(const AddOrder&) = default;
        AddOrder& operator=(AddOrder&&) noexcept = default;

        /**
         * Overload new/delete to use the mempool, rather than heap allocations
         */
        void* operator new(size_t) {
            void* ptr = _mempool.allocate();
            if (ptr == nullptr) throw std::bad_alloc();
            return ptr;
        }

        void operator delete(void* ptr) {
            _mempool.deallocate(static_cast<AddOrder*>(ptr));
        }

        bool processMessage() const override { OrderBook::getInstance().addToActiveOrders(orderReferenceNumber, header.getStockLocate(), shares, price); return true; }

        // Setters
        void setOrderReferenceNumber(uint64_t orderReferenceNumber) { this -> orderReferenceNumber = orderReferenceNumber; }
        void setBuySellIndicator(char buySellIndicator) { this -> buySellIndicator = buySellIndicator; }
        void setShares(uint32_t shares) { this -> shares = shares; }
        void setStock(std::string stock) { this -> stock = std::move(stock); }
        void setPrice(uint32_t price) { this -> price = price; }

        // Getters
        uint64_t getOrderReferenceNumber() const { return this -> orderReferenceNumber; }
        char getBuySellIndicator() const { return this -> buySellIndicator; }
        uint32_t getShares() const { return this -> shares; }
        std::string getStock() const { return this -> stock; }
        uint32_t getPrice() const { return this -> price; }
};

AddOrder* parseAddOrderBody(BinaryMessageHeader header, const char* data);

#endif // NASDAQ_MESSAGES_ADD_ORDER_H_