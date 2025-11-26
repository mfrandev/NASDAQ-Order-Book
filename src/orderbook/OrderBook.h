#ifndef ORDER_BOOK_H_
#define ORDER_BOOK_H_

#include <ankerl/unordered_dense.h>

#include <OrderBookEntry.h>
#include <LedgerEntry.h>
#include <OrderBookConstants.h>
#include <ThreadConstants.h>

class OrderBook {
    
    // orderReferencenumber -> entry
    using OrderBookMap = ankerl::unordered_dense::map<uint64_t, OrderBookEntry>;

    // matchNumber -> entry
    using Ledger = ankerl::unordered_dense::map<uint64_t, LedgerEntry>;

    private:

        static OrderBook* _instance; 
        OrderBookMap _orderBook;
        Ledger _ledger;

        void _addOrderToOrderBook(uint16_t stockLocate, uint32_t shares, uint32_t price, uint64_t orderReferenceNumber);
        void _removeOrderFromOrderBook(uint64_t orderReferenceNumber);
        void _replaceKeyInOrderBook(uint64_t oldOrderReferenceNumber, uint64_t newOrderReferenceNumber);
        void _addExecutedTradeToLedger(uint16_t stockLocate, uint32_t price, uint64_t numShares, uint64_t matchNumber);

    public:    

        static OrderBook& getInstance();

        void addOrder(uint16_t stockLocate, uint32_t shares, uint32_t price, uint64_t orderReferenceNumber);
        uint32_t executeOrder(uint32_t numExecutedShares, uint64_t orderReferenceNumber, uint64_t matchNumber);
        void executeOrderWithPrice(uint32_t executionPrice, uint32_t numExecutedShares, uint64_t orderReferenceNumber, uint64_t matchNumber);
        void cancelOrder(uint32_t numSharesCancelled, uint64_t orderReferenceNumber); // Partial cancellation 
        void deleteOrder(uint64_t orderReferenceNumber);
        void replaceOrder(uint32_t newPrice, uint32_t newShares, uint64_t oldOrderReferenceNumber, uint64_t newOrderReferenceNumber);

        // TODO: Determine if using optimal space reservation strategy
        OrderBook() { 
            _orderBook.reserve(OrderBookConstants::RESERVE_ENTRIES);
            _orderBook.max_load_factor(0.7);
            _ledger.reserve(OrderBookConstants::RESERVE_ENTRIES / NUMBER_OF_SHARDS);
            _ledger.max_load_factor(0.7);
        }
        ~OrderBook() = default;
        OrderBook(const OrderBook&) = default;
        OrderBook(OrderBook&&) noexcept = default;
        OrderBook& operator=(const OrderBook&) = default;
        OrderBook& operator=(OrderBook&&) noexcept = default;

};

#endif // ORDER_BOOK_H_