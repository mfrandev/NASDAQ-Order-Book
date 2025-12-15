# NASDAQ ITCH 5.0 Order Book 

## Project Goal: With maximum efficiency, parse a day's worth of NASDAQ ITCH 5.0 trading data, and maintain an order book while calculating real-time trading metrics, which minimally includes VWAP (volume-weighted average price).

## This repository contains an improved implementation upon my previous attempt at this project: <a href="https://github.com/mfrandev/NASDAQ-Order-Book-Old">NASDAQ-Order-Book-Old (found here)</a>

### Total execution time for this version of the project is ~40% faster than the old version (~20 seconds vs ~34 seconds), given my day-long ITCH 5.0 sample file.

Main Features:
<ol>
    <li>Constructs an Order Book from a day's worth of NASDAQ ITCH 5.0 trading data.</li>
    <li>Maintains a trade ledger to permit broken trade correction for trading statistics.</li>
    <li>Calculates VWAP for each listed stock IN REAL TIME.</li>
    <li>Supports VWAP queries over the last N minutes for any security.</li>
</ol>

<br>

Main Performance Gains:
<ol>
    <li>This implementation uses a lockfree SPMC multithreading model, or "single producer, multi consumer."</li>
    <li>All program threads are carefully pinned to MY SPECIFIC INTEL MACHINE'S P-cores, or "performance cores"</li>
    <li>Inter-thread communication is facilitated using Erik Rigtorp's Lockfree, SPSC Queue (see this wonderful <a href="https://rigtorp.se/ringbuffer/">blog post here</a>>) </li>
    <li>Message types are implemented as tagged unions to bypass heavyweight polymorphism.</li>
    <li>C++ move semantics are carefully considered when making choices in this implementation.</li>
</ol>

Remaining TODOs:
<ol>
    <li>Query interface is non-existant. Remove hard coded VWAP queries for "AAPL" and "XOM" and support user defined queries.</li>
    <li>Create a comprehensive output format for the results of these queries.</li>
    <li>Investigate a better thread backoff strategy beyond "std::this_thread::yield()"</li>
</ol>

### More details coming soon!