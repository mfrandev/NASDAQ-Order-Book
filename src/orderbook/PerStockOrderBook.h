#ifndef PER_STOCK_ORDER_BOOK_H_
#define PER_STOCK_ORDER_BOOK_H_

#include <ankerl/unordered_dense.h>
#include <ostream>

#include <PerStockOrderBookEntry.h>
#include <PerStockOrderBookConstants.h>
#include <ThreadConstants.h>

class PerStockOrderBook {
    
    // orderReferencenumber -> entry
    using PerStockOrderBookMap = ankerl::unordered_dense::map<uint64_t, PerStockOrderBookEntry>;

    private:

        PerStockOrderBookMap _perStockOrderBook;

        void _addOrderToOrderBook(uint32_t shares, uint32_t price, uint64_t orderReferenceNumber);
        void _removeOrderFromOrderBook(uint64_t orderReferenceNumber);
        void _replaceKeyInOrderBook(uint64_t oldOrderReferenceNumber, uint64_t newOrderReferenceNumber);

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
        }

};

#endif // PER_STOCK_ORDER_BOOK_H_
