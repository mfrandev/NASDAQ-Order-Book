#ifndef NASDAQ_MESSAGES_ADD_ORDER_MPID_H_
#define NASDAQ_MESSAGES_ADD_ORDER_MPID_H_

#include <string>
#include <cstdint>

#include <Message.h>

#include <OrderBook.h>

#include <mempool.h>

// constexpr size_t BUY_SELL_INDICATOR_SIZE = 1; Use the value from AddOrder.h
// constexpr size_t STOCK_SIZE = 8; Use the value from StockTradingAction.h
constexpr size_t ATTRIBUTION_SIZE = 4;

// Store the Add Order MPID message body
class AddOrderMPID : public Message {
    private:
        uint64_t orderReferenceNumber;
        char buySellIndicator;
        uint32_t shares;
        std::string stock;
        uint32_t price;
        std::string attribution;

        static lockfree::MempoolSPSC<AddOrderMPID, SPSC_QUEUE_CAPACITY + 2> _mempool;
    
    public:
        /**
         * Rule-of-5 compliant
         */
        AddOrderMPID(BinaryMessageHeader header, uint64_t orderReferenceNumber, char buySellIndicator, uint32_t shares, std::string stock, uint32_t price, std::string attribution) :
            Message(std::move(header)),
            orderReferenceNumber(orderReferenceNumber),
            buySellIndicator(buySellIndicator),
            shares(shares),
            stock((std::move(stock))),
            price(price),
            attribution(std::move(attribution))
            {}

            bool processMessage() const override { OrderBook::getInstance().addToActiveOrders(orderReferenceNumber, header.getStockLocate(), shares, price); return true; }

            /**
             * Overload new/delete to use the mempool, rather than heap allocations
             */
            void* operator new(size_t) {
                void* ptr = _mempool.allocate();
                if (ptr == nullptr) throw std::bad_alloc();
                return ptr;
            }

            void operator delete(void* ptr) {
                _mempool.deallocate(static_cast<AddOrderMPID*>(ptr));
            }

            // Setters
            void setOrderReferenceNumber(uint64_t orderReferenceNumber) { this -> orderReferenceNumber = orderReferenceNumber; }
            void setBuySellIndicator(char buySellIndicator) { this -> buySellIndicator = buySellIndicator; }
            void setShares(uint32_t shares) { this -> shares = shares; }
            void setStock(std::string stock) { this -> stock = std::move(stock); }
            void setPrice(uint32_t price) { this -> price = price; }
            void setAttribution(std::string attribution) { this -> attribution = std::move(attribution); }

            // Getters
            uint64_t getOrderReferenceNumber() const { return this -> orderReferenceNumber; }
            char getBuySellIndicator() const { return this -> buySellIndicator; }
            uint32_t getShares() const { return this -> shares; }
            std::string getStock() const { return this -> stock; }
            uint32_t getPrice() const { return this -> price; }
            std::string getAttribution() const { return this -> attribution; }
};

AddOrderMPID* parseAddOrderMPIDBody(BinaryMessageHeader header, const char* data);

#endif // NASDAQ_MESSAGES_ADD_ORDER_MPID_H_