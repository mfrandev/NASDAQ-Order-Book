#ifndef LEDGER_ENTRY_H_
#define LEDGER_ENTRY_H_

#include <cstdint>

struct LedgerEntry {
    uint16_t stockLocate;
    uint32_t price;
    uint64_t numShares;
};

#endif // LEDGER_ENTRY_H_