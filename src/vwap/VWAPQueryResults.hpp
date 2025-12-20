#ifndef VWAP_QUERY_RESULTS_
#define VWAP_QUERY_RESULTS_

#include <cstdint>
#include <cmath>

struct VWAPIntervalQueryResult {
    uint64_t startNs;
    uint64_t endNs;
    uint64_t deltaNumShares;
    __uint128_t deltaNotional;
};

// Minor loss of accuracy, but ok for project purposes
inline double vwapAsDouble(const VWAPIntervalQueryResult& query) {
    double vwap = (static_cast<long double>(query.deltaNotional) /
           static_cast<long double>(query.deltaNumShares)) / 10'000;
    return std::round(vwap * 100.00) / 100.00;
}

#endif // VWAP_QUERY_RESULTS_
