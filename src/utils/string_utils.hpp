#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include <cstddef>
#include <cctype>
#include <string>
#include <algorithm>

/**
 * Although not necessary, I like knowing I have this utility
 */
inline std::string charStarToString(const char* data, size_t size) {
    return std::string(data, size);
}

/**
 * This utility removes all whitespace from a cpp string in-place
 */
inline void stripWhitespaceFromCPPString(std::string& str) {
    str.erase(
        std::remove_if(
            str.begin(), 
            str.end(), ::isspace
        ), 
        str.end()
    );
}

/**
 * This utility turns a 16 byte integer to a string for printing, since __uint128_t does not have operator<< defined
 */
inline std::string uint128_t_to_string(__uint128_t value) {
    if(value == 0) return "0";
    std::string valueAsString;
    while(value > 0) {
        auto digit = static_cast<char>('0' + (value % 10));
        valueAsString.push_back(digit);
        value /= 10;
    }
    std::reverse(valueAsString.begin(), valueAsString.end());
    return valueAsString;
}

#endif // STRING_UTILS_H_
