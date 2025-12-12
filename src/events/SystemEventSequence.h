#ifndef SYSTEM_EVENT_STATE_
#define SYSTEM_EVENT_STATE_

#include <cstdint>
#include <atomic>

struct SystemEventSequence {
    std::atomic<uint64_t> startOfMessagesSeq    = __UINT64_MAX__;
    std::atomic<uint64_t> startOfSystemHoursSeq = __UINT64_MAX__;
    std::atomic<uint64_t> startOfMarketHoursSeq = __UINT64_MAX__;
    std::atomic<uint64_t> endOfMarketHoursSeq   = __UINT64_MAX__;
    std::atomic<uint64_t> endOfSystemHoursSeq   = __UINT64_MAX__;
    std::atomic<uint64_t> endOfMessagesSeq      = __UINT64_MAX__;
}; 

#endif // SYSTEM_EVENT_STATE_