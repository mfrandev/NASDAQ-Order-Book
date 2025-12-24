#ifndef PER_STOCK_REALIZED_VARIANCE_
#define PER_STOCK_REALIZED_VARIANCE_

#include <cstdint>
#include <cmath>

#include <PerStockRealizedVarianceCorrection.h>
#include <PerStockRealizedVariance.hpp>

struct PerStockRV {
    bool hasLastPrice{false};
    double rvSum{0.0};
    double cachedLastLogPrice{0.0};
};

inline void accumulateRV(uint32_t newPrice, PerStockRV& rv) {
    if(newPrice == 0) return;
    double logPrice = std::log(static_cast<double>(newPrice));
    if(rv.hasLastPrice) {
        double diffLogPrices = logPrice - rv.cachedLastLogPrice;
        rv.rvSum += (diffLogPrices * diffLogPrices);
    }
    rv.cachedLastLogPrice = logPrice;
    rv.hasLastPrice = true;
}

inline void correctRV(PerStockRV& rv, const PerStockRVCorrection& rvCorrection) {
    rv.rvSum -= rvCorrection.oldNextSquaredLogDiff;
    rv.rvSum += rvCorrection.newNextSquaredLogDiff;
    rv.rvSum -= rvCorrection.brokenSquaredLogDiff;
}

#endif // PER_STOCK_REALIZED_VARIANCE_
