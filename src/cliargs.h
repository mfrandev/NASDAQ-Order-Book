#ifndef CLI_ARGS_
#define CLI_ARGS_

struct Statistics {
    bool includeVWAP = false;
};

struct Intervals {
    uint64_t intervalVWAP = 30;
};

struct CLIArgs {
    Statistics statistics;
    Intervals intervals;
};

#endif // CLI_ARGS_
