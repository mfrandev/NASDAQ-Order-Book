#ifndef VWAP_QUERY_RESULTS_
#define VWAP_QUERY_RESULTS_

#include <cstdint>

struct VWAPIntervalQueryResult {
    uint64_t startNs;
    uint64_t endNs;
    uint64_t numShares;
    __uint128_t notional;
};

// Minor loss of accuracy, but ok for project purposes
inline double vwapAsDouble(const VWAPIntervalQueryResult& query) {
    double vwap = (static_cast<long double>(query.notional) /
           static_cast<long double>(query.numShares)) / 10'000;
    return std::round(vwap * 100.00) / 100.00;
}

#endif // VWAP_QUERY_RESULTS_
