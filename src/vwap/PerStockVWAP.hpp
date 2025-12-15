#ifndef PER_STOCK_VWAP_
#define PER_STOCK_VWAP_

#include <PerStockVWAPCorrection.h>

struct PerStockVWAP {
    uint64_t numShares{0};
    __uint128_t notional{0};
};

inline void accumulateVWAP(PerStockVWAP& vwap, uint32_t price, uint64_t shares) {
    vwap.numShares += shares;
    vwap.notional += static_cast<__uint128_t>(price) * shares;
}

inline void correctVWAP(PerStockVWAP& vwap, const PerStockVWAPCorrection& correction) {
    vwap.numShares -= correction.shares;
    vwap.notional -= correction.notional;
}

#endif // PER_STOCK_VWAP_
