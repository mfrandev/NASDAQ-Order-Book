#ifndef PER_STOCK_VWAP_
#define PER_STOCK_VWAP_

#include <cstdint>

struct PerStockVWAP {
    uint64_t numShares{0};
    __uint128_t notional{0};
};

#endif // PER_STOCK_VWAP_