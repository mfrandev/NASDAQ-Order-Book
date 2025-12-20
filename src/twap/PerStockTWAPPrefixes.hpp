#ifndef PER_STOCK_TWAP_PREFIXES_
#define PER_STOCK_TWAP_PREFIXES_

#include <cstdint>
#include <vector>
#include <algorithm>
#include <optional>

#include <PerStockTWAPConstants.h>
#include <PerStockTWAP.hpp>
#include <TWAPQueryResults.hpp>

struct PerStockTWAPPrefix {

    uint64_t snapshotNs{0};
    uint64_t totalTime{0}; 
    __uint128_t priceTime{0}; // sum of price * delta t

};

class PerStockTWAPPrefixHistory {

    private:
        std::vector<PerStockTWAPPrefix> twapPrefixes;
        uint64_t currentSnapshotNs{0};

        // Find the first TWAP
        PerStockTWAPPrefix findPrefixAtOrBeforeTimestamp(uint64_t timestamp) const {
            if(twapPrefixes.empty()) return {0, 0, 0};
            auto firstStrictlyGreaterThanTimestamp = std::upper_bound(
                twapPrefixes.begin(),
                twapPrefixes.end(),
                timestamp,
                [](uint64_t t, const PerStockTWAPPrefix& item) {
                    return t < item.snapshotNs;
                }
            );
            if(firstStrictlyGreaterThanTimestamp == twapPrefixes.begin()) return {0, 0, 0};
            --firstStrictlyGreaterThanTimestamp;
            return *firstStrictlyGreaterThanTimestamp;
        }

    public:

        PerStockTWAPPrefixHistory() {
            twapPrefixes.reserve(TWAPConstants::TWAP_BUCKET_COUNT);
        }

        

        void maybeAdvanceTWAPBucket(uint64_t timestamp, PerStockTWAP& liveTWAP) {
            if(currentSnapshotNs == 0) {
                currentSnapshotNs = (timestamp / TWAPConstants::NANOSECOND_PER_MINUTE) * TWAPConstants::NANOSECOND_PER_MINUTE;
                return;
            }

            while(timestamp >= currentSnapshotNs + TWAPConstants::NANOSECOND_PER_MINUTE) {
                uint64_t boundaryNs = currentSnapshotNs + TWAPConstants::NANOSECOND_PER_MINUTE;
                accumulateTWAP(liveTWAP, liveTWAP.lastPrice, boundaryNs);
                twapPrefixes.emplace_back(
                    boundaryNs,
                    liveTWAP.totalTime,
                    liveTWAP.priceTime
                );
                currentSnapshotNs = boundaryNs;
            }
        }

        void flush(uint64_t timestamp, PerStockTWAP& liveTWAP) {
            if(currentSnapshotNs == 0) return;

            maybeAdvanceTWAPBucket(timestamp, liveTWAP);

            accumulateTWAP(liveTWAP, liveTWAP.lastPrice, timestamp);
            twapPrefixes.emplace_back(
                timestamp,
                liveTWAP.totalTime,
                liveTWAP.priceTime
            );
            currentSnapshotNs = timestamp;
        }

        std::optional<TWAPIntervalQueryResult> queryInterval(uint64_t start, uint64_t end) const {
            if(end <= start) return std::nullopt;
            PerStockTWAPPrefix intervalStart = findPrefixAtOrBeforeTimestamp(start);
            PerStockTWAPPrefix intervalEnd = findPrefixAtOrBeforeTimestamp(end);
            if(intervalEnd.totalTime < intervalStart.totalTime) return std::nullopt;
            uint64_t deltaTotalTime = intervalEnd.totalTime - intervalStart.totalTime;
            if(deltaTotalTime == 0) return std::nullopt;
            if(intervalEnd.priceTime < intervalStart.priceTime) return std::nullopt;
            __uint128_t deltaPriceTime = intervalEnd.priceTime - intervalStart.priceTime;
            return TWAPIntervalQueryResult {
                start, 
                end,
                deltaTotalTime,
                deltaPriceTime
            };
        }

        
        std::optional<TWAPIntervalQueryResult> queryAsOfInterval(uint64_t startNs, uint64_t endNs, PerStockTWAP& asOfInterval) const {
            if(endNs <= startNs) return std::nullopt;
            PerStockTWAPPrefix intervalStart = findPrefixAtOrBeforeTimestamp(startNs);
            PerStockTWAPPrefix intervalEnd = findPrefixAtOrBeforeTimestamp(endNs);

            uint64_t endTotalTime;
            __uint128_t endPriceTime;

            // Use temporary As-of accumulator
            if(intervalEnd.snapshotNs < endNs) {
                endTotalTime = asOfInterval.totalTime;
                endPriceTime = asOfInterval.priceTime;
            } else {
                endTotalTime = intervalEnd.totalTime;
                endPriceTime = intervalEnd.priceTime;
            }

            if(endTotalTime < intervalStart.totalTime) return std::nullopt;
            uint64_t deltaTotalTime = endTotalTime - intervalStart.totalTime;
            if(deltaTotalTime == 0) return std::nullopt;
            if(endPriceTime < intervalStart.priceTime) return std::nullopt;
            __uint128_t deltaPriceTime = endPriceTime - intervalStart.priceTime;
            return TWAPIntervalQueryResult {
                startNs, 
                endNs,
                deltaTotalTime,
                deltaPriceTime
            };
        }

        void processTWAPUpdate(uint32_t newPrice, uint64_t newTimestamp, PerStockTWAP& twap) {
            maybeAdvanceTWAPBucket(newTimestamp, twap);
            accumulateTWAP(twap, newPrice, newTimestamp);
        }

};

#endif // PER_STOCK_TWAP_PREFIXES_
