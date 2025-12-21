#ifndef TWAP_QUERY_RESULTS_
#define TWAP_QUERY_RESULTS_

#include <cstdint>
#include <cmath>

#include <PerStockTWAPConstants.h>

struct TWAPIntervalQueryResult {
    uint64_t startNs{0};
    uint64_t endNs{0};
    uint64_t deltaTotalTime{0};
    __uint128_t deltaPriceTime{0};
};

// Minor loss of accuracy, but ok for project purposes
inline double twapAsDouble(const TWAPIntervalQueryResult& query) {
    if(query.deltaTotalTime == 0) return 0.0;
    double twap = (static_cast<long double>(query.deltaPriceTime) /
           static_cast<long double>(query.deltaTotalTime)) / TWAPConstants::SHIFT_FOUR_DECIMAL_PLACES;
    return std::round(twap * 100.00) / 100.00;
}

#endif // TWAP_QUERY_RESULTS_
