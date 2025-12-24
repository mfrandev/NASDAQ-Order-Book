#ifndef PER_STOCK_REALIZED_VARIANCE_CONSTANTS_
#define PER_STOCK_REALIZED_VARIANCE_CONSTANTS_

#include <cstdint>

namespace PerStockRVConstants {
    constexpr uint64_t NANOSECOND_PER_MINUTE = 60'000'000'000;
    constexpr uint32_t RV_BUCKET_COUNT = 512;
    constexpr uint64_t SHIFT_FOUR_DECIMAL_PLACES = 10'000UL;
};

#endif // PER_STOCK_REALIZED_VARIANCE_CONSTANTS_
