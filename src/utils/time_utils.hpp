#ifndef TIME_UTILS_
#define TIME_UTILS_

#include <SystemEventSequence.h>

bool oustide_of_system_hours(SystemEventSequence* eventSeqTruthSource, uint64_t seq) {
    return seq < eventSeqTruthSource->startOfSystemHoursSeq.load(std::memory_order_acquire) 
    || seq > eventSeqTruthSource->endOfSystemHoursSeq.load(std::memory_order_acquire);
}

bool oustide_of_market_hours(SystemEventSequence* eventSeqTruthSource, uint64_t seq) {
    return seq < eventSeqTruthSource->startOfMarketHoursSeq.load(std::memory_order_acquire) 
    || seq > eventSeqTruthSource->endOfMarketHoursSeq.load(std::memory_order_acquire);
}

#endif // TIME_UTILS_