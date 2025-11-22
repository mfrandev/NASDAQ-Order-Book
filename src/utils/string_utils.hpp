#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include <cstddef>
#include <string>
#include <algorithm>

/**
 * Although not necessary, I like knowing I have this utility
 */
std::string charStarToString(const char* data, size_t size) {
    return std::string(data, size);
}

/**
 * This utility removes all whitespace from a cpp string in-place
 */
void stripWhitespaceFromCPPString(std::string& str) {
    str.erase(
        std::remove_if(
            str.begin(), 
            str.end(), ::isspace
        ), 
        str.end()
    );
}

#endif // STRING_UTILS_H_