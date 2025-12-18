#ifndef PER_STOCK_ORDER_STATE_CONSTANTS_H_
#define PER_STOCK_ORDER_STATE_CONSTANTS_H_

#include <cstddef>

namespace PerStockOrderStateConstants {
    // TODO: This is subject to further investigation
    constexpr size_t RESERVE_ENTRIES = 8'192;
    constexpr size_t NUM_OF_STOCK_LOCATE = UINT16_MAX + 1;
};

#endif // PER_STOCK_ORDER_STATE_CONSTANTS_H_