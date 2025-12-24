#ifndef CLI_ARGS_
#define CLI_ARGS_

struct Metrics {
    bool includeVWAP = false;
    bool includeTWAP = false;
    bool includeRV = false;
};

struct Intervals {
    uint64_t intervalVWAP = 30;
    uint64_t intervalTWAP = 30;
    uint64_t intervalRV = 30;
};

struct CLIArgs {
    Metrics metrics;
    Intervals intervals;
};

#endif // CLI_ARGS_
