#include <PerStockLedger.h>

#include <algorithm>
#include <cmath>

#include <string_utils.hpp>
#include <logarithm_price_cache_utils.hpp>

LedgerAddResult PerStockLedger::addExecutedTradeToLedger(bool countedInMetrics, uint32_t price, uint64_t numShares, uint64_t matchNumber) {
    auto [ledgerIt, inserted] = _perStockLedger.try_emplace(matchNumber, _entries.size());
    if(inserted) {
        _entries.emplace_back();
        ledgerIt->second = _entries.size() - 1;
    }
    PerStockLedgerEntry& entry = _entries[ledgerIt->second];
    __uint128_t notional = static_cast<__uint128_t>(price) * numShares;
    entry.totalNotionalPrice += notional;
    entry.totalShares += numShares;

    LedgerAddResult ledgerAddState;

    if(countedInMetrics && price != 0) {
        // VWAP accounting should occur for every match number
        entry.vwapNotionalPrice += notional;
        entry.vwapShares += numShares;

        ledgerAddState.vwapUpdated = true;

        // RV is path based, so only account once per match number
        if(entry.linkedIntoRV) {
            assert(entry.logPriceScaled == LogPriceCache::getInstance().getLogScaled(price));
            return ledgerAddState;
        }

        ledgerAddState.rvLinkCreated = true;
        entry.linkedIntoRV = true;

        entry.logPriceScaled = LogPriceCache::getInstance().getLogScaled(price); // 0.0 default

        entry.nextIndex = INVALID_INDEX;
        entry.previousIndex = lastIndex;

        if(lastIndex != INVALID_INDEX) {
            PerStockLedgerEntry& lastEntry = _entries[lastIndex];
            lastEntry.nextIndex = ledgerIt->second;
            double logDiff = (entry.logPriceScaled - lastEntry.logPriceScaled);
            entry.squaredLogDiff = logDiff * logDiff;
        } else {
            entry.squaredLogDiff = 0.0;
        }
        lastIndex = ledgerIt->second;
    }

    return ledgerAddState;

}

std::optional<PerStockCorrection> PerStockLedger::handleBrokenTradeOrOrderExecution(uint64_t matchNumber) {
    auto ledgerIterator = _perStockLedger.find(matchNumber);
    if(ledgerIterator == _perStockLedger.end())
        return std::nullopt;
    const uint64_t entryIndex = ledgerIterator->second;
    PerStockLedgerEntry& entry = _entries[entryIndex];
    PerStockVWAPCorrection vwapCorrection {
        entry.vwapShares,
        entry.vwapNotionalPrice
    };

    PerStockRVCorrection rvCorrection{};

    if(entry.linkedIntoRV) {

        // Reorganize linked list structure
        if(entry.previousIndex != INVALID_INDEX) {
            _entries[entry.previousIndex].nextIndex = entry.nextIndex;
        }
        if(entry.nextIndex != INVALID_INDEX) {
            _entries[entry.nextIndex].previousIndex = entry.previousIndex;
        }
        if(lastIndex == entryIndex) {
            lastIndex = entry.previousIndex;
        }

        rvCorrection.brokenSquaredLogDiff = entry.squaredLogDiff;

        if(entry.nextIndex != INVALID_INDEX) {
            PerStockLedgerEntry& nextEntry = _entries[entry.nextIndex];
            rvCorrection.oldNextSquaredLogDiff = nextEntry.squaredLogDiff;

            if(entry.previousIndex != INVALID_INDEX) {
                PerStockLedgerEntry& previousEntry = _entries[entry.previousIndex];
                double diff = nextEntry.logPriceScaled - previousEntry.logPriceScaled;
                nextEntry.squaredLogDiff = diff * diff;
                rvCorrection.newNextSquaredLogDiff = nextEntry.squaredLogDiff;
            } else {
                // This becomes the first trade in the RV path
                rvCorrection.newNextSquaredLogDiff = 0.0;
                nextEntry.squaredLogDiff = 0.0;
            }
        }
    }

    entry.nextIndex = INVALID_INDEX;
    entry.previousIndex = INVALID_INDEX;
    entry.linkedIntoRV = false;

    _perStockLedger.erase(ledgerIterator);
    return PerStockCorrection{ rvCorrection, vwapCorrection };
}

void PerStockLedger::dump(std::ostream& os) const {
    std::vector<std::pair<uint64_t, uint64_t>> ledger;
    ledger.reserve(_perStockLedger.size());
    for (const auto& kv : _perStockLedger) {
        ledger.push_back(kv);
    }
    std::sort(ledger.begin(), ledger.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    os << "=== Executed Trades Ledger (" << ledger.size() << ") ===\n";
    for (const auto& [matchNumber, entryIndex] : ledger) {
        const PerStockLedgerEntry& entry = _entries[entryIndex];
        const auto shares = entry.totalShares;
        const std::string priceStr = (shares == 0)
            ? "0"
            : uint128_t_to_string(entry.totalNotionalPrice / static_cast<__uint128_t>(shares));
        os << "matchNumber=" << matchNumber << " price=" << priceStr << " shares=" << shares << '\n';
    }
}
