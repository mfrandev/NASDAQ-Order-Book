#ifndef NASDAQ_BOOKKEEPING_ORDER_BOOK_H
#define NASDAQ_BOOKKEEPING_ORDER_BOOK_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <type_traits>

#include <absl/container/node_hash_map.h>
#include <absl/container/flat_hash_map.h>
#include <robin_map.h>
#include <unordered_dense.h>

#include <mempool.h>

#include <spdlog/spdlog.h>

constexpr uint64_t ORDER_BOOK_BUCKET_RESERVATION = 11'184'811; // A number I've found works well. Orginally discovered using ((BYTES_TO_MB * 128) / sizeof(ActiveOrderData))
constexpr uint64_t ACTIVE_ORDER_MEMPOOL_SIZE = 12'000'000; // Needs to be adequately proportioned w.r.t. the amount of reserved order book memory (prev: 6'000'000)


/**
 * Book has two mappings:
 * 1. Order Reference Number -> ActiveOrderData
 * 2. Match Number -> ExecutedOrderOrTradeData
 * (Since Orders and Trades can be retroactively broken, must maintain a collection of executed Orders and Trades)
 */
struct ExecutedOrderOrTradeData {
    uint16_t stockLocate;
    uint32_t numShares;
    uint32_t executionPrice;
    uint8_t executionPeriod;
};

class ActiveOrderData {

    private: 
        uint32_t numShares;
        uint32_t price;
        uint16_t stockLocate;
        bool deleted;

    public:

        ActiveOrderData(uint16_t stockLocate, uint32_t numShares, uint32_t price)
            : stockLocate(stockLocate), numShares(numShares), price(price), deleted(false) {
                // spdlog::get("console") -> info("Offset of numShares: {}", offsetof(ActiveOrderData, numShares));
                // spdlog::get("console") -> info("Offset of price {}", offsetof(ActiveOrderData, price));
                // spdlog::get("console") -> info("Offset of stockLocate: {}", offsetof(ActiveOrderData, stockLocate));
                // spdlog::get("console") -> info("Offset of pack: {}", offsetof(ActiveOrderData, pack));
                // spdlog::get("console") -> info("Size of ActiveOrderData: {}", sizeof(ActiveOrderData));

            }
        ~ActiveOrderData() = default;
        ActiveOrderData(const ActiveOrderData&) = default;
        ActiveOrderData& operator=(const ActiveOrderData&) = default;
        ActiveOrderData(ActiveOrderData&&) noexcept = default;
        ActiveOrderData& operator=(ActiveOrderData&&) noexcept = default;
        ActiveOrderData() 
            : stockLocate(0), numShares(0), price(0), deleted(false) {}

        // Getters for the private members
        uint16_t getStockLocate() const { return stockLocate; }
        uint32_t getNumShares() const { return numShares; }
        uint32_t getPrice() const { return price; }
        bool getDeleted() const { return deleted; }
        void setNumShares(uint32_t newNumShares) { numShares = newNumShares; }
        void setPrice(uint32_t newPrice) { price = newPrice; }
        void setStockLocate(uint16_t newStockLocate) { stockLocate = newStockLocate; }
        void setDeleted(bool newDeleted) { deleted = newDeleted; }
};

static_assert(std::is_standard_layout<ActiveOrderData>::value);

template<typename T>
class OrderBookAllocator {
    public:

        using value_type = T;

        OrderBookAllocator() noexcept = default;

        template<typename U>
        OrderBookAllocator(const OrderBookAllocator<U>& other) noexcept {}

        T* allocate(size_t size) {
            return _mempool.allocate();
        }

        void deallocate(T* ptr, size_t size) noexcept {
            _mempool.deallocate(ptr);
        }   

        template<typename U>
        bool operator ==(const OrderBookAllocator<U>& other) const noexcept {
            return true;
        }

        template<typename U>
        bool operator !=(const OrderBookAllocator<U>& other) const noexcept {
            return !(*this == other);
        }

    private:
        static lockfree::MempoolSPSC<T, ACTIVE_ORDER_MEMPOOL_SIZE> _mempool;
};


/**
 * This class is responsible for maintaining the order book
 */
class OrderBook {
    public:

        static OrderBook& getInstance();

        // Reserve 64 MB of memory for this map
        OrderBook() {
            _activeOrdersBook.reserve(ORDER_BOOK_BUCKET_RESERVATION);
            _activeOrdersBook.max_load_factor(1.0);
            // spdlog::get("console") -> info ("pair is {} bytes", sizeof(std::pair<const uint64_t, ActiveOrderData>));
        }

        // Functions for maintaining the book
        void addToActiveOrders(uint64_t orderReferenceNumber, uint16_t stockLocate, uint32_t numShares, uint32_t price);
        uint32_t executeActiveOrder(uint64_t orderReferenceNumber, uint32_t numExecutedShares, uint64_t matchNumber);
        void executeActiveOrderWithPrice(uint64_t orderReferenceNumber, uint32_t numExecutedShares, uint64_t matchNumber, uint32_t price);
        void cancelActiveOrder(uint64_t orderReferenceNumber, uint32_t numCancelledShares);
        void deleteActiveOrder(uint64_t orderReferenceNumber);
        void replaceActiveOrder(uint16_t stockLocate, uint64_t originalOrderReferenceNumber, uint64_t newOrderReferenceNumber, uint32_t newNumShares, uint32_t newPrice);
        
    private:

        static OrderBook* _instance;

        /**
         * 1. This maps an Order Reference Number to an active order
         * 2. <k,v> mappings in this may get updated or removed throughout the day
         */
        std::unordered_map
        <
            uint64_t, 
            ActiveOrderData, 
            std::hash<uint64_t>, 
            std::equal_to<uint64_t>, 
            OrderBookAllocator<std::pair<const uint64_t, ActiveOrderData>>
        > 
        _activeOrdersBook;

        // absl::node_hash_map<uint64_t, ActiveOrderData> _activeOrdersBook;
        // absl::flat_hash_map<uint64_t, ActiveOrderData> _activeOrdersBook;
        // tsl::robin_map<uint64_t, ActiveOrderData> _activeOrdersBook;
        // ankerl::unordered_dense::map<uint64_t, ActiveOrderData> _activeOrdersBook;

};

#endif // NASDAQ_BOOKKEEPING_ORDER_BOOK_H