#ifndef PER_STOCK_ORDER_BOOK_H_
#define PER_STOCK_ORDER_BOOK_H_

#include <ankerl/unordered_dense.h>
#include <ostream>

#include <PerStockOrderBookEntry.h>
#include <PerStockLedgerEntry.h>
#include <PerStockOrderBookConstants.h>
#include <ThreadConstants.h>

class PerStockOrderBook {
    
    // orderReferencenumber -> entry
    using PerStockOrderBookMap = ankerl::unordered_dense::map<uint64_t, PerStockOrderBookEntry>;

    // matchNumber -> entry
    using PerStockLedger = ankerl::unordered_dense::map<uint64_t, PerStockLedgerEntry>;

    private:

        PerStockOrderBookMap _perStockOrderBook;
        PerStockLedger _perStockLedger;

        void _addOrderToOrderBook(uint32_t shares, uint32_t price, uint64_t orderReferenceNumber);
        void _removeOrderFromOrderBook(uint64_t orderReferenceNumber);
        void _replaceKeyInOrderBook(uint64_t oldOrderReferenceNumber, uint64_t newOrderReferenceNumber);
        void _addExecutedTradeToLedger(uint32_t price, uint64_t numShares, uint64_t matchNumber);

    public:    

        void addOrder(uint32_t shares, uint32_t price, uint64_t orderReferenceNumber);
        uint32_t executeOrder(uint32_t numExecutedShares, uint64_t orderReferenceNumber, uint64_t matchNumber);
        void executeOrderWithPrice(uint32_t executionPrice, uint32_t numExecutedShares, uint64_t orderReferenceNumber, uint64_t matchNumber);
        void cancelOrder(uint32_t numSharesCancelled, uint64_t orderReferenceNumber); // Partial cancellation 
        void deleteOrder(uint64_t orderReferenceNumber);
        void replaceOrder(uint32_t newPrice, uint32_t newShares, uint64_t oldOrderReferenceNumber, uint64_t newOrderReferenceNumber);
        void dump(std::ostream& os) const;

        // TODO: Determine if using optimal space reservation strategy
        PerStockOrderBook() { 
            _perStockOrderBook.reserve(PerStockOrderBookConstants::RESERVE_ENTRIES);
            _perStockOrderBook.max_load_factor(0.7);
            _perStockLedger.reserve(PerStockOrderBookConstants::RESERVE_ENTRIES / NUMBER_OF_SHARDS);
            _perStockLedger.max_load_factor(0.7);
        }
        ~PerStockOrderBook() = default;
        PerStockOrderBook(const PerStockOrderBook&) = default;
        PerStockOrderBook(PerStockOrderBook&&) noexcept = default;
        PerStockOrderBook& operator=(const PerStockOrderBook&) = default;
        PerStockOrderBook& operator=(PerStockOrderBook&&) noexcept = default;

};

#endif // PER_STOCK_ORDER_BOOK_H_
