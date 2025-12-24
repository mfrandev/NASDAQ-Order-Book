#ifndef PER_STOCK_REALIZED_VARIANCE_CORRECTION_
#define PER_STOCK_REALIZED_VARIANCE_CORRECTION_

struct PerStockRVCorrection {
    double oldNextSquaredLogDiff{0.0};
    double newNextSquaredLogDiff{0.0};
    double brokenSquaredLogDiff{0.0};
};

static_assert(sizeof(PerStockRVCorrection) == 24);

#endif // PER_STOCK_REALIZED_VARIANCE_CORRECTION_