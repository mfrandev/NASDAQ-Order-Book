add_library(murmur3 STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../lib/murmur3/murmur3.c)
include_directories(
    "./../lib/murmur3"
    "./../lib/er"
    "./../lib/CLI11"
    "."
    "./messages"
    "./orderstate"
    "./utils"
    "./shard"
    "./events"
    "./ledger"
    "./vwap"
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# The compiled library code is here
add_executable(nasdaq_statistics_engine ${SOURCES})
target_link_libraries(nasdaq_statistics_engine PRIVATE unordered_dense::unordered_dense spdlog::spdlog murmur3)
target_compile_options(
    nasdaq_statistics_engine
    PRIVATE
        # $<$<CONFIG:Debug>:-O0 -g3 -fno-omit-frame-pointer>
        -march=native # for __sync_val_compare_and_swap
        --param=destructive-interference-size=64
)
