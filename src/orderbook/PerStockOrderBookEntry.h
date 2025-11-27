#ifndef PER_STOCK_ORDER_BOOK_ENTRY_H_
#define PER_STOCK_ORDER_BOOK_ENTRY_H_

#include <cstdint>

struct PerStockOrderBookEntry {
    uint32_t shares;
    uint32_t price;
};

#endif // PER_STOCK_ORDER_BOOK_ENTRY_H_