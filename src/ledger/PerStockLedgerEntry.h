#ifndef PER_STOCK_LEDGER_ENTRY_H_
#define PER_STOCK_LEDGER_ENTRY_H_

#include <cstdint>

// VWAP fields are updated only for executions that represent public prints.
// Non-printable executions still update total execution fields for rollback.
struct PerStockLedgerEntry {
    uint64_t totalShares{0};
    uint64_t vwapShares{0};

    __uint128_t totalNotionalPrice{0};
    __uint128_t vwapNotionalPrice{0};
};

static_assert(sizeof(PerStockLedgerEntry) == 48);

#endif // PER_STOCK_LEDGER_ENTRY_H_
