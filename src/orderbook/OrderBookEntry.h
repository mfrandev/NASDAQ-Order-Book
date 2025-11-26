#ifndef ORDER_BOOK_ENTRY_H_
#define ORDER_BOOK_ENTRY_H_

#include <cstdint>

struct OrderBookEntry {
    uint16_t stockLocate;
    uint32_t shares;
    uint32_t price;
};

#endif // ORDER_BOOK_ENTRY_H_