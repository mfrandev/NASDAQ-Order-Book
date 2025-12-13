#ifndef PER_STOCK_ORDER_BOOK_ENTRY_H_
#define PER_STOCK_ORDER_BOOK_ENTRY_H_

#include <cstdint>

struct PerStockOrderBookEntry {
    uint32_t shares{0};
    uint32_t price{0};
};

#endif // PER_STOCK_ORDER_BOOK_ENTRY_H_