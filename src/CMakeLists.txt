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

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# The compiled library code is here
add_executable(nasdaq_order_book ${SOURCES})
target_link_libraries(nasdaq_order_book PRIVATE spdlog::spdlog)
target_compile_options(nasdaq_order_book PRIVATE -march=native) # for __sync_val_compare_and_swap