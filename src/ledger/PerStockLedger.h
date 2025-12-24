#ifndef PER_STOCK_LEDGER_
#define PER_STOCK_LEDGER_

#include <ostream>
#include <optional>
#include <vector>
#include <limits>

#include <ankerl/unordered_dense.h>

#include <PerStockVWAPCorrection.h>
#include <PerStockRealizedVarianceCorrection.h>
#include <PerStockLedgerEntry.h>
#include <PerStockLedgerConstants.h>

struct PerStockCorrection {
    PerStockRVCorrection rvCorrection;
    PerStockVWAPCorrection vwapCorrection;
};

struct LedgerAddResult {
    bool vwapUpdated{false};
    bool rvLinkCreated{false};
};

class PerStockLedger {

    // matchNumber -> entry
    using PerStockLedgerMap = ankerl::unordered_dense::map<uint64_t, uint64_t>;

    private:
        static constexpr uint64_t INVALID_INDEX = std::numeric_limits<uint64_t>::max();

        PerStockLedgerMap _perStockLedger;
        std::vector<PerStockLedgerEntry> _entries;
        uint64_t lastIndex{INVALID_INDEX};

    public:

        PerStockLedger() {
            _perStockLedger.reserve(PerStockLedgerConstants::RESERVE_ENTRIES);
            _perStockLedger.max_load_factor(0.7);
            _entries.reserve(PerStockLedgerConstants::RESERVE_ENTRIES);
        }

        LedgerAddResult addExecutedTradeToLedger(bool countedInMetrics, uint32_t price, uint64_t numShares, uint64_t matchNumber);
        std::optional<PerStockCorrection> handleBrokenTradeOrOrderExecution(uint64_t matchNumber);
        void dump(std::ostream& os) const;

};

#endif // PER_STOCK_LEDGER_
