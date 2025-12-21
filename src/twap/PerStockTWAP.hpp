#ifndef PER_STOCK_TWAP_
#define PER_STOCK_TWAP_

#include <cstdint>

struct PerStockTWAP {
    uint32_t lastPrice{0}; 
    uint64_t lastTimestamp{0};
    uint64_t totalTime{0};    // sum of delta t
    __uint128_t priceTime{0}; // sum of price * delta t
};

inline void accumulateTWAP(PerStockTWAP& twap, uint32_t newPrice, uint64_t newTimestamp) {
    // Don't destroy averages by considering "null" price/timestamp at initialization
    if(twap.lastTimestamp == 0) {
        twap.lastPrice = newPrice;
        twap.lastTimestamp = newTimestamp;
        return;
    }
    if(newTimestamp < twap.lastTimestamp) return; // Found non-monotonic timestamp pair
    auto deltaTime = newTimestamp - twap.lastTimestamp;
    twap.priceTime += static_cast<__uint128_t>(twap.lastPrice) * static_cast<__uint128_t>(deltaTime);
    twap.totalTime += deltaTime;
    
    twap.lastPrice = newPrice;
    twap.lastTimestamp = newTimestamp;
}

#endif // PER_STOCK_TWAP_
