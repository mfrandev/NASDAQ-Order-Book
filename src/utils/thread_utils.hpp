#ifndef THREAD_UTILS_H_
#define THREAD_UTILS_H_

#include <pthread.h>
#include <assert.h>

bool pin_to_core(int core) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);
    int success = pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    assert(success == 0);
    return success == 0;
}

#endif // THREAD_UTILS_H_