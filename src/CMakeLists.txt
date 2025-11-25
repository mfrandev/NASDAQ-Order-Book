add_library(murmur3 STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../lib/murmur3/murmur3.c)
include_directories(
    "./../lib/readerwritercircularbuffer"
    "./../lib/murmur3"
    "./../lib/er"
    # "./../lib/concurrentqueue"
    "."
    "./messages"
    "./utils"
    "./shard"
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# The compiled library code is here
add_executable(nasdaq_order_book_mpmc ${SOURCES})
target_link_libraries(nasdaq_order_book_mpmc PRIVATE spdlog::spdlog murmur3)
target_compile_options(nasdaq_order_book_mpmc PRIVATE -march=native) # for __sync_val_compare_and_swap