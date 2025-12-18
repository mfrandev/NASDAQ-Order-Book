#ifndef PER_STOCK_ORDER_STATE_H_
#define PER_STOCK_ORDER_STATE_H_

#include <ankerl/unordered_dense.h>
#include <ostream>

#include <PerStockOrderStateEntry.h>
#include <PerStockOrderStateConstants.h>
#include <ThreadConstants.h>

class PerStockOrderState {
    
    // orderReferencenumber -> entry
    using PerStockOrderStateMap = ankerl::unordered_dense::map<uint64_t, PerStockOrderStateEntry>;

    private:

        PerStockOrderStateMap _perStockOrderState;

        void _addOrderToOrderState(uint32_t shares, uint32_t price, uint64_t orderReferenceNumber);
        void _removeOrderFromOrderState(uint64_t orderReferenceNumber);
        void _replaceKeyInOrderState(uint64_t oldOrderReferenceNumber, uint64_t newOrderReferenceNumber);

    public:    

        void addOrder(uint32_t shares, uint32_t price, uint64_t orderReferenceNumber);
        uint32_t executeOrder(uint32_t numExecutedShares, uint64_t orderReferenceNumber, uint64_t matchNumber);
        void executeOrderWithPrice(uint32_t executionPrice, uint32_t numExecutedShares, uint64_t orderReferenceNumber, uint64_t matchNumber);
        void cancelOrder(uint32_t numSharesCancelled, uint64_t orderReferenceNumber); // Partial cancellation 
        void deleteOrder(uint64_t orderReferenceNumber);
        void replaceOrder(uint32_t newPrice, uint32_t newShares, uint64_t oldOrderReferenceNumber, uint64_t newOrderReferenceNumber);
        void dump(std::ostream& os) const;

        // TODO: Determine if using optimal space reservation strategy
        PerStockOrderState() { 
            _perStockOrderState.reserve(PerStockOrderStateConstants::RESERVE_ENTRIES);
            _perStockOrderState.max_load_factor(0.7);
        }

};

#endif // PER_STOCK_ORDER_STATE_H_
