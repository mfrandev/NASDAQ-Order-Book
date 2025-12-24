#ifndef PER_STOCK_LEDGER_ENTRY_H_
#define PER_STOCK_LEDGER_ENTRY_H_

#include <cstdint>
#include <limits>

// VWAP fields are updated only for executions that represent public prints.
// Non-printable executions still update total execution fields for rollback.
struct PerStockLedgerEntry {

    // log(Price * 1e-4)
    bool linkedIntoRV{false};
    double logPriceScaled{0.0};
    double squaredLogDiff{0.0};
    uint64_t previousIndex{std::numeric_limits<uint64_t>::max()};
    uint64_t nextIndex{std::numeric_limits<uint64_t>::max()};

    uint64_t totalShares{0};
    uint64_t vwapShares{0};
    __uint128_t totalNotionalPrice{0};
    __uint128_t vwapNotionalPrice{0};
};

static_assert(sizeof(PerStockLedgerEntry) == 96);
static_assert(alignof(PerStockLedgerEntry) >= 8);

#endif // PER_STOCK_LEDGER_ENTRY_H_
