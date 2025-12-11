#ifndef THREAD_CONSTANTS_H_
#define THREAD_CONSTANTS_H_

#include <cstdint>

// MY OWN LOCAL MACHINE HAS 6 P-CORES AVAILABLE FOR CONSUMER THREADS: CPUs 4,6,8,10,12,14
constexpr uint8_t NUMBER_OF_SHARDS = 6;

// TODO: Tune shard size. 
constexpr uint32_t SHARD_SIZE = 4096;

namespace ThreadConstants {
    constexpr uint16_t BUSY_WAIT_CYCLES = 75;
    constexpr uint8_t  YIELD_CYCLES = 5;
    constexpr uint8_t FIRST_CONSUMER_CORE = 4;
};

#endif // THREAD_CONSTANTS_H_