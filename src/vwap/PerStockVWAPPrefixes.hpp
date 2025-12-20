#ifndef PER_STOCK_VWAP_PREFIX_HISTORY_
#define PER_STOCK_VWAP_PREFIX_HISTORY_

#include <cstdint>
#include <vector>
#include <optional>
#include <algorithm>

#include <PerStockVWAPConstants.h>
#include <VWAPQueryResults.hpp>
#include <PerStockVWAP.hpp>

struct PerStockVWAPPrefix {
    uint64_t bucketKeyNs{0};
    uint64_t numShares{0};
    __uint128_t notional{0};
};

class PerStockVWAPPrefixHistory {

    private:
        std::vector<PerStockVWAPPrefix> vwapPrefixes;
        uint64_t currentBucketKeyNs{0};

        // Find the first VWAP
        PerStockVWAPPrefix findPrefixAtOrBeforeTimestamp(uint64_t timestamp) const {
            if(vwapPrefixes.empty()) return {0, 0, 0};
            auto firstStrictlyGreaterThanTimestamp = std::upper_bound(
                vwapPrefixes.begin(),
                vwapPrefixes.end(),
                timestamp,
                [](uint64_t t, const PerStockVWAPPrefix& item) {
                    return t < item.bucketKeyNs;
                }
            );
            if(firstStrictlyGreaterThanTimestamp == vwapPrefixes.begin()) return {0, 0, 0};
            --firstStrictlyGreaterThanTimestamp;
            return *firstStrictlyGreaterThanTimestamp;
        }

    public:
        PerStockVWAPPrefixHistory() {
            vwapPrefixes.reserve(VWAPConstants::VWAP_BUCKET_COUNT);
        }

        void maybeAdvanceVWAPBucket(uint64_t timestamp, const PerStockVWAP& liveVWAP) {

            // Get the start of the "true" current bucket for this message
            uint64_t bucketKeyNs = (timestamp / VWAPConstants::NANOSECOND_PER_MINUTE) * VWAPConstants::NANOSECOND_PER_MINUTE;

            // If current bucket start is unset (first bucket), set it and return
            if(currentBucketKeyNs == 0) {
                currentBucketKeyNs = bucketKeyNs;
                return;
            }

            // If "current" bucket is moved on from, save the state for the previous prefix
            if(currentBucketKeyNs != bucketKeyNs) {
                vwapPrefixes.emplace_back(
                    currentBucketKeyNs,
                    liveVWAP.numShares,
                    liveVWAP.notional
                );
                currentBucketKeyNs = bucketKeyNs;
            }
        }
        
        void flush(const PerStockVWAP& liveVWAP) {
            if(currentBucketKeyNs == 0) return;
            vwapPrefixes.emplace_back(
                currentBucketKeyNs,
                liveVWAP.numShares,
                liveVWAP.notional
            );
        }

        void processVWAPUpdate(uint32_t price, uint32_t shares, uint64_t timestamp, PerStockVWAP& vwap) {
            maybeAdvanceVWAPBucket(timestamp, vwap);
            accumulateVWAP(vwap, price, shares);
        }

        std::optional<VWAPIntervalQueryResult> queryInterval(uint64_t start, uint64_t end) const {
            if(end <= start) return std::nullopt;
            PerStockVWAPPrefix intervalStart = findPrefixAtOrBeforeTimestamp(start);
            PerStockVWAPPrefix intervalEnd = findPrefixAtOrBeforeTimestamp(end);
            if(intervalEnd.numShares < intervalStart.numShares) return std::nullopt;
            uint64_t deltaShares = intervalEnd.numShares - intervalStart.numShares;
            if(deltaShares == 0) return std::nullopt;
            if(intervalEnd.notional < intervalStart.notional) return std::nullopt;
            __uint128_t deltaNotional = intervalEnd.notional - intervalStart.notional;
            return VWAPIntervalQueryResult {
                start,
                end,
                deltaShares,
                deltaNotional
            };
        }

};

#endif // PER_STOCK_VWAP_PREFIX_HISTORY_
