#ifndef REALIZED_VARIANCE_QUERY_RESULTS_
#define REALIZED_VARIANCE_QUERY_RESULTS_

#include <cstdint>

struct RVIntervalQueryResult {
    double deltaRVSum{0.0};
    uint64_t startNs{0};
    uint64_t endNs{0};
};

#endif // REALIZED_VARIANCE_QUERY_RESULTS_