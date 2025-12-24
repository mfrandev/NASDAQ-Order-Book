#ifndef TIME_UTILS_
#define TIME_UTILS_

#include <string>

#include <SystemEventSequence.h>

bool outside_of_system_hours(SystemEventSequence* eventSeqTruthSource, uint64_t seq) {
    return seq < eventSeqTruthSource->startOfSystemHoursSeq.load(std::memory_order_acquire) 
    || seq > eventSeqTruthSource->endOfSystemHoursSeq.load(std::memory_order_acquire);
}

bool outside_of_market_hours(SystemEventSequence* eventSeqTruthSource, uint64_t seq) {
    return seq < eventSeqTruthSource->startOfMarketHoursSeq.load(std::memory_order_acquire) 
    || seq > eventSeqTruthSource->endOfMarketHoursSeq.load(std::memory_order_acquire);
}

std::string toTimeString(uint64_t timestampNs, char delimiter) {
            uint64_t totalSeconds = timestampNs / 1'000'000'000ULL;
            uint64_t hours = totalSeconds / 3600;
            uint64_t minutes = (totalSeconds % 3600) / 60;
            uint64_t seconds = totalSeconds % 60;
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(2) << hours << delimiter
                << std::setw(2) << minutes << delimiter
                << std::setw(2) << seconds;
            return oss.str();
        };

#endif // TIME_UTILS_