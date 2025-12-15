#ifndef PER_STOCK_VWAP_CORRECTION_
#define PER_STOCK_VWAP_CORRECTION_

#include <cstdint>

struct PerStockVWAPCorrection {
    uint64_t shares;
    __uint128_t notional;
};

#endif // PER_STOCK_VWAP_CORRECTION_