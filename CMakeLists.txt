cmake_minimum_required(VERSION 3.15...3.25)

project(
    NASDAQ-Order-Book
    DESCRIPTION "VWAP from Binary ITCH 5.0 File"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

execute_process(
  COMMAND ${CMAKE_CXX_COMPILER} -v
  ERROR_VARIABLE COMPILER_INFO
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Underlying compiler: ${COMPILER_INFO}")

add_subdirectory(lib/spdlog)
add_subdirectory(lib/abseil-cpp)
add_subdirectory(lib/unordered_dense)

add_subdirectory(src)

