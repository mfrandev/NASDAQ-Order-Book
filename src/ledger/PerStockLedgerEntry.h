#ifndef PER_STOCK_LEDGER_ENTRY_H_
#define PER_STOCK_LEDGER_ENTRY_H_

#include <cstdint>

// VWAP fields are updated only for executions that represent public prints.
// Non-printable executions still update total execution fields for rollback.
struct PerStockLedgerEntry {
    uint64_t totalShares;
    uint64_t vwapShares;

    __uint128_t totalNotionalPrice;
    __uint128_t vwapNotionalPrice;
};

static_assert(sizeof(PerStockLedgerEntry) == 48);

#endif // PER_STOCK_LEDGER_ENTRY_H_