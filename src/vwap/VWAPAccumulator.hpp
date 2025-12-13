#ifndef VWAP_ACCUMULATOR_
#define VWAP_ACCUMULATOR_

#include <PerStockVWAP.h>
#include <PerStockVWAPCorrection.h>

void accumulateVWAP(PerStockVWAP& vwap, uint32_t price, uint64_t shares) {
    vwap.numShares += shares;
    vwap.notional += static_cast<__uint128_t>(price) * shares;
}

void correctVWAP(PerStockVWAP& vwap, const PerStockVWAPCorrection& correction) {
    vwap.numShares -= correction.shares;
    vwap.notional -= correction.notional;
}

#endif // VWAP_ACCUMULATOR_