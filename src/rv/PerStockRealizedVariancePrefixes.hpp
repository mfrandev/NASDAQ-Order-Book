#ifndef PER_STOCK_REALIZED_VARIANCE_PREFIXES_
#define PER_STOCK_REALIZED_VARIANCE_PREFIXES_

#include <cstdint>
#include <vector>
#include <optional>
#include <algorithm>

#include <PerStockRealizedVarianceConstants.h>
#include <PerStockRealizedVariance.hpp>
#include <RealizedVarianceQueryResults.hpp>

struct PerStockRVPrefix {
    double rvSum{0};
    uint64_t snapshotNs{0};
};

class PerStockRVPrefixHistory {

    private:
        std::vector<PerStockRVPrefix> rvPrefixes;
        uint64_t currentSnapshotNs{0};

        PerStockRVPrefix findPrefixAtOrBeforeTimestamp(uint64_t timestamp) const {
            if(rvPrefixes.empty()) return {0, 0};
            auto firstStrictlyGreaterThanTimestamp = std::upper_bound(
                rvPrefixes.begin(),
                rvPrefixes.end(),
                timestamp,
                [](uint64_t t, const PerStockRVPrefix& item) {
                    return t < item.snapshotNs;
                }
            );

            // If timestamp comes before first prefix, RV is defined as 0
            if(firstStrictlyGreaterThanTimestamp == rvPrefixes.begin()) return {0, 0};
            --firstStrictlyGreaterThanTimestamp;
            return *firstStrictlyGreaterThanTimestamp;
        }

    public:
        PerStockRVPrefixHistory() {
            rvPrefixes.reserve(PerStockRVConstants::RV_BUCKET_COUNT);
        }

        void maybeAdvanceRVBucket(uint64_t timestamp, const PerStockRV& liveRV) {
            if(currentSnapshotNs == 0) {
                currentSnapshotNs = (timestamp / PerStockRVConstants::NANOSECOND_PER_MINUTE) * PerStockRVConstants::NANOSECOND_PER_MINUTE;
            }
            while(timestamp >= currentSnapshotNs + PerStockRVConstants::NANOSECOND_PER_MINUTE) { 
                currentSnapshotNs += PerStockRVConstants::NANOSECOND_PER_MINUTE;
                
                // This should "close" the current bucket since we use time boundaries
                rvPrefixes.emplace_back(
                    liveRV.rvSum,
                    currentSnapshotNs
                );
            }
        }

        std::optional<RVIntervalQueryResult> queryAsOfInterval(uint64_t startNs, uint64_t endNs, const PerStockRV& asOfInterval) const {
            if(endNs <= startNs) return std::nullopt;
            PerStockRVPrefix intervalStart = findPrefixAtOrBeforeTimestamp(startNs);
            PerStockRVPrefix intervalEnd = findPrefixAtOrBeforeTimestamp(endNs);

            double endRVSum = 
            (intervalEnd.snapshotNs < endNs)
                ? asOfInterval.rvSum
                : intervalEnd.rvSum;

            if(endRVSum < intervalStart.rvSum) return std::nullopt;

            return RVIntervalQueryResult {
                endRVSum - intervalStart.rvSum,
                startNs,
                endNs
            };
        }

};

#endif // PER_STOCK_REALIZED_VARIANCE_PREFIXES_
