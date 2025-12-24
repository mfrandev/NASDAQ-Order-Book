#ifndef LOGARITHM_PRICE_CACHE_UTILS_
#define LOGARITHM_PRICE_CACHE_UTILS_

#include <cmath>
#include <cstdint>
#include <shared_mutex>
#include <mutex>

#include <cassert>

#include <ankerl/unordered_dense.h>

constexpr double SCALING_FACTOR = 1e-4;

class LogPriceCache {

    private:
        ankerl::unordered_dense::map<uint32_t, double> logCache;
        std::shared_mutex mutex;
        LogPriceCache() {
        logCache.reserve(1 << 16);
    }

    public:

    static LogPriceCache& getInstance() {
        static LogPriceCache instance;
        return instance;
    }

    double getLogScaled(uint32_t price) {
        assert(price > 0);

        {
            std::shared_lock lock(mutex);
            auto it = logCache.find(price);
            if(it != logCache.end())
                return it->second;
        }
        
        {
            std::unique_lock lock(mutex);
            auto it = logCache.find(price);
            if(it != logCache.end())
                return it->second;
            double logVal = std::log(static_cast<double>(price) * SCALING_FACTOR);
            logCache.emplace(price, logVal);
            return logVal;
        }
    }

};

#endif // LOGARITHM_PRICE_CACHE_UTILS_