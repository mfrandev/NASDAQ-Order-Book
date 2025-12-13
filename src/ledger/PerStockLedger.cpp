#include <PerStockLedger.h>

#include <algorithm>

#include <string_utils.hpp>

void PerStockLedger::addExecutedTradeToLedger(bool countedInMetrics, uint32_t price, uint64_t numShares, uint64_t matchNumber) {
    PerStockLedgerEntry& entry = _perStockLedger[matchNumber];
    __uint128_t notional = static_cast<__uint128_t>(price) * numShares;
    entry.totalNotionalPrice += notional;
    entry.totalShares += numShares;

    if(countedInMetrics) {
        entry.vwapNotionalPrice += notional;
        entry.vwapShares += numShares;
    }
}

PerStockVWAPCorrection PerStockLedger::handleBrokenTradeOrOrderExecution(uint64_t matchNumber) {
    auto ledgerIterator = _perStockLedger.find(matchNumber);
    if(ledgerIterator == _perStockLedger.end())
        return PerStockVWAPCorrection{0, 0};
    PerStockLedgerEntry& entry = ledgerIterator -> second;
    PerStockVWAPCorrection correction {
        entry.vwapShares,
        entry.vwapNotionalPrice
    };
    _perStockLedger.erase(ledgerIterator);
    return correction;
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
        const auto shares = entry.totalShares;
        const std::string priceStr = (shares == 0)
            ? "0"
            : uint128_t_to_string(entry.totalNotionalPrice / static_cast<__uint128_t>(shares));
        os << "matchNumber=" << matchNumber << " price=" << priceStr << " shares=" << shares << '\n';
    }
}
