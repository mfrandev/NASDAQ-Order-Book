#ifndef ENDIAN_UTILS_H_
#define ENDIAN_UTILS_H_

#include <cstdint>
#include <cstring>

/**
 * The binary file stores integers big-endian, and x86 is little endian.
 * Here is a utility function to convery big-endian to little-endian
 */

#if __BYTE_ORDER == __LITTLE_ENDIAN
/**
 * The binary file stores integers big-endian, and x86 is little endian.
 * Here is a utility function to convery big-endian to little-endian
 */
uint64_t toHostEndianUpTo64(const char* data, size_t size) {
    uint64_t result = 0;

    // shift to make the LSB of "data" the MSB of "result"
    for (size_t i = 0; i < size; ++i) {
        result |= static_cast<uint64_t>(static_cast<uint8_t>(data[i])) << ((size - 1 - i) * 8);
    }
    return result;
}

#else
/**
 * If the host machine is already big-endian, do nothing special
 */
uint64_t toHostEndianUpTo64(const char* data, size_t size) {
    uint64_t result;
    std::memcmp(&result, &data[0], size);
    return result;
}

#endif

#endif // ENDIAN_UTILS_H_