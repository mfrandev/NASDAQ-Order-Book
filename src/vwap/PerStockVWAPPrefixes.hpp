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
    uint64_t bucketStartNs{0};
    uint64_t numShares{0};
    __uint128_t notional{0};
};

class PerStockVWAPPrefixHistory {

    private:
        std::vector<PerStockVWAPPrefix> vwapPrefixes;
        uint64_t currentBucketStartNs{0};

        // Find the first VWAP
        PerStockVWAPPrefix findPrefixAtOrBeforeTimestamp(uint64_t timestamp) const {
            if(vwapPrefixes.empty()) return {0, 0, 0};
            auto firstStrictlyGreaterThanTimestamp = std::upper_bound(
                vwapPrefixes.begin(),
                vwapPrefixes.end(),
                timestamp,
                [](uint64_t t, const PerStockVWAPPrefix& item) {
                    return t < item.bucketStartNs;
                }
            );
            if(firstStrictlyGreaterThanTimestamp == vwapPrefixes.begin()) return {0, 0, 0};
            --firstStrictlyGreaterThanTimestamp;
            return *firstStrictlyGreaterThanTimestamp;
        }

    public:
        PerStockVWAPPrefixHistory() {
            vwapPrefixes.reserve(VWAP_BUCKET_COUNT);
        }

        void maybeAdvanceVWAPBucket(uint64_t timestamp, const PerStockVWAP& liveVWAP) {

            // Get the start of the "true" current bucket for this message
            uint64_t bucketStartNs = (timestamp / NANOSECOND_PER_MINUTE) * NANOSECOND_PER_MINUTE;

            // If current bucket start is unset (first bucket), set it and return
            if(currentBucketStartNs == 0) {
                currentBucketStartNs = bucketStartNs;
                return;
            }

            // If "current" bucket is moved on from, save the state for the previous prefix
            if(currentBucketStartNs != bucketStartNs) {
                vwapPrefixes.emplace_back(
                    currentBucketStartNs,
                    liveVWAP.numShares,
                    liveVWAP.notional
                );
                currentBucketStartNs = bucketStartNs;
            }
        }
        
        void flush(const PerStockVWAP& liveVWAP) {
            if(currentBucketStartNs == 0) return;
            vwapPrefixes.emplace_back(
                currentBucketStartNs,
                liveVWAP.numShares,
                liveVWAP.notional
            );
        }

        std::optional<VWAPIntervalQueryResult> queryInterval(uint64_t start, uint64_t end) const {
            if(end <= start) return std::nullopt;
            PerStockVWAPPrefix intervalStart = findPrefixAtOrBeforeTimestamp(start);
            PerStockVWAPPrefix intervalEnd = findPrefixAtOrBeforeTimestamp(end);
            if(intervalEnd.numShares < intervalStart.numShares) return std::nullopt;
            uint64_t deltaShares = intervalEnd.numShares - intervalStart.numShares;
            if(intervalEnd.notional < intervalStart.notional) return std::nullopt;
            __uint128_t deltaNotional = intervalEnd.notional - intervalStart.notional;
            if(deltaShares == 0) return std::nullopt;
            return VWAPIntervalQueryResult {
                start,
                end,
                deltaShares,
                deltaNotional
            };
        }

};

#endif // PER_STOCK_VWAP_PREFIX_HISTORY_
