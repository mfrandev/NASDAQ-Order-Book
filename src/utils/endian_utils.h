#ifndef NASDAQ_UTILS_ENDIAN_UTILS_H_
#define NASDAQ_UTILS_ENDIAN_UTILS_H_

#include <cstdint>
#include <cstring>
#include <stdlib.h>

/**
 * The binary file stores integers big-endian, and x86 is little endian.
 * Here is a utility function to convery big-endian to little-endian
 */
uint64_t toHostEndianUpTo64(const char* data, size_t size);

#endif // NASDAQ_UTILS_ENDIAN_UTILS_H_