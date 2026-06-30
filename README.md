# NASDAQ Metrics Engine

A high-performance C++20 application that parses binary NASDAQ ITCH 5.0 trading data, maintains a real-time order book, and calculates intraday trading metrics (VWAP, TWAP, and Realized Variance). This implementation is ~40% faster than its [predecessor (NOW PRIVATE)](https://github.com/mfrandev/NASDAQ-Order-Book-Old) (~20s vs ~34s on a full trading day file).

## Preliminary Notes

- Input file path is hardcoded and must be changed at the source level.
- Thread core pinning is tuned for my specific Intel x86_64 machine with 8 p-core families. Adjust `FIRST_CONSUMER_CORE` to map to your own hardware topology for more optimal performance.
- No interactive query interface; metric intervals are fixed at startup via CLI flags.

## Features

- **Trade Ledger**: Tracks executed trades and supports broken trade correction for retroactive metric adjustment.
- **VWAP**: Volume-weighted average price, queryable over the last N minutes per security. Uses 128-bit integers to prevent overflow on notional values.
- **TWAP**: Time-weighted average price, queryable over configurable intervals.
- **Realized Variance**: Return volatility from intraday price movements, computed over configurable intervals.
- **Lock-free SPMC multithreading**: Single producer (file reader) distributes messages to multiple consumer threads via lock-free SPSC queues (Erik Rigtorp's implementation). Consumer threads are pinned to P-cores families for minimal latency.

## Architecture

```
Main Thread (Producer)
  └── Reads binary ITCH 5.0 file sequentially
  └── Routes messages to per-shard queues by applyig the murmur3 hash function to each message's stock locate (ticker ID)

Consumer Threads (6 workers, pinned to P-cores)
  └── Each thread processes a subset of stocks
  └── Maintains order book, trade ledger, and metric accumulators
  └── Writes results to CSV files in LiveOutput/
  └── The threading model is not generic and was fine tuned for my local machine's CPU topology.
```

Messages are represented as tagged unions to avoid virtual dispatch overhead. Each metric uses a 512-bucket prefix-sum structure for O(1) range queries over time windows.

## Project Structure

```
src/
├── main.cpp                  # Entry point, CLI parsing, thread orchestration
├── messages/                 # ITCH 5.0 protocol parsing (header, message types, constants)
├── shard/                    # Producer/consumer threading (ShardManager, ShardConsumer)
├── orderstate/               # Per-stock order book state
├── ledger/                   # Trade ledger with broken trade correction
├── vwap/                     # VWAP accumulators, prefix sums, correction logic
├── twap/                     # TWAP accumulators and prefix sums
├── rv/                       # Realized variance accumulators and prefix sums
├── events/                   # System event tracking (market session lifecycle)
└── utils/                    # Endian conversion, string/time utilities, queue wrapper
lib/
├── spdlog/                   # Logging
├── unordered_dense/          # High-performance hash map (ankerl::unordered_dense)
├── CLI11/                    # CLI argument parsing
├── murmur3/                  # Hash function
└── er/                       # Lock-free SPSC queue
LiveOutput/
├── vwap/                     # VWAP CSV output
├── twap/                     # TWAP CSV output
└── rv/                       # Realized variance CSV output
```

## Building

Requires x86 architecture, a C++20 compiler, and CMake 3.15+. External dependencies are included as git submodules.

```bash
git submodule update --init --recursive
./buildit.sh
```

Or manually:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The build uses `-march=native`, so binaries are optimized for the machine they're compiled on.

## Running

```bash
./runit.sh
```

Or directly:

```bash
./build/src/nasdaq_metrics_engine -v --vwap-interval 30 -t --twap-interval 10 -r --rv-interval 20
```

**Note**: The input file path is hardcoded in `src/main.cpp` as `../../ITCHFiles/01302019.NASDAQ_ITCH50`. Update this path to point to your ITCH 5.0 binary file before building.

Sample data is available from Nasdaq's [ITCH data archive](https://emi.nasdaq.com/ITCH/Nasdaq%20ITCH/01302020.NASDAQ_ITCH50.gz). Download and decompress it into `ITCHFiles/` at the project root.

## CLI Options

| Flag | Description |
|------|-------------|
| `-v`, `--vwap` | Enable VWAP calculation |
| `--vwap-interval N` | VWAP query interval in minutes (requires `-v`) |
| `-t`, `--twap` | Enable TWAP calculation |
| `--twap-interval N` | TWAP query interval in minutes (requires `-t`) |
| `-r`, `--rv` | Enable Realized Variance calculation |
| `--rv-interval N` | Realized Variance query interval in minutes (requires `-r`) |

## Output

Results are written as CSV files to `LiveOutput/{vwap,twap,rv}/`. One file is produced per reporting interval, containing per-stock metric values for that window.

## Configuration

Compile-time constants in `src/shard/ThreadConstants.h`:

| Constant | Default | Description |
|----------|---------|-------------|
| `NUMBER_OF_SHARDS` | 6 | Number of consumer threads |
| `SHARD_SIZE` | 4096 | Per-queue message capacity |
| `FIRST_CONSUMER_CORE` | 4 | First P-core to pin consumers to |
| `BUSY_WAIT_CYCLES` | 75 | Spin iterations before yielding |
| `YIELD_CYCLES` | 5 | Yield iterations before sleeping |

## Dependencies

| Library | Purpose |
|---------|---------|
| [spdlog](https://github.com/gabime/spdlog) | Logging |
| [unordered_dense](https://github.com/martinus/unordered_dense) | High-performance hash map |
| [CLI11](https://github.com/CLIUtils/CLI11) | CLI argument parsing |
| [murmur3](https://github.com/aappleby/smhasher) | Hash function |
| [SPSCQueue](https://github.com/rigtorp/SPSCQueue) | Lock-free single-producer single-consumer queue |

## License

[Unlicense](LICENSE) — public domain.
