#ifndef PER_STOCK_LEDGER_ENTRY_H_
#define PER_STOCK_LEDGER_ENTRY_H_

#include <cstdint>

struct PerStockLedgerEntry {
    uint32_t notionalPrice;
    uint64_t numShares;
};

#endif // PER_STOCK_LEDGER_ENTRY_H_