#ifndef PER_STOCK_LEDGER_
#define PER_STOCK_LEDGER_

#include <ankerl/unordered_dense.h>
#include <ostream>

#include <PerStockLedgerEntry.h>
#include <PerStockLedgerConstants.h>

class PerStockLedger {

    // matchNumber -> entry
    using PerStockLedgerMap = ankerl::unordered_dense::map<uint64_t, PerStockLedgerEntry>;

    private:
        PerStockLedgerMap _perStockLedger;

    public:

        PerStockLedger() {
            _perStockLedger.reserve(PerStockLedgerConstants::RESERVE_ENTRIES);
            _perStockLedger.max_load_factor(0.7);
        }

        void addExecutedTradeToLedger(uint32_t price, uint64_t numShares, uint64_t matchNumber);
        void dump(std::ostream& os) const;

};

#endif // PER_STOCK_LEDGER_