#include <PerStockLedger.h>
#include <algorithm>

void PerStockLedger::addExecutedTradeToLedger(uint32_t price, uint64_t numShares, uint64_t matchNumber) {
    PerStockLedgerEntry& entry = _perStockLedger[matchNumber];
    entry.notionalPrice += (price * numShares);
    entry.numShares += numShares;
}

void PerStockLedger::dump(std::ostream& os) const {
    std::vector<std::pair<uint64_t, PerStockLedgerEntry>> ledger;
    ledger.reserve(_perStockLedger.size());
    for (const auto& kv : _perStockLedger) {
        ledger.push_back(kv);
    }
    std::sort(ledger.begin(), ledger.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    os << "=== Executed Trades Ledger (" << ledger.size() << ") ===\n";
    for (const auto& [matchNumber, entry] : ledger) {
        os << "matchNumber=" << matchNumber << " price=" << (entry.notionalPrice / entry.numShares) << " shares=" << entry.numShares << '\n';
    }
}