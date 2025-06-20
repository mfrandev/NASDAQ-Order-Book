cmake_minimum_required(VERSION 3.15...3.25)

project(
    NASDAQ-Order-Book
    DESCRIPTION "VWAP from Binary ITCH 5.0 File"
    LANGUAGES CXX
)

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

find_package(Boost REQUIRED)

include_directories(
    "."
    "../lib/CLI11"
    "../lib/lockfree_mpmc_queue"
    "./bookkeeping" 
    "./messages"
    "./utils"
    "./data_structures"
    ${Boost_INCLUDE_DIRS}
)

file(GLOB SOURCES
  "main.cpp"
  "../lib/CLI11/*.hpp"
  "../lib/lockfree_mpmc_queue/*.h"
  "./bookkeeping/*.cpp"
  "./messages/*.cpp"
  "./utils/*.cpp"
  "./data_structures/*.cpp"
)

# The compiled library code is here
add_executable(nasdaq_order_book ${SOURCES})
target_link_libraries(nasdaq_order_book fmt::fmt)
target_compile_options(nasdaq_order_book PRIVATE -march=native) # for __sync_val_compare_and_swap