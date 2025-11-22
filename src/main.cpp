#include <fstream>
#include <vector>
#include <chrono>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <MessageHeader.h>

#include <message_utils.hpp>

const std::string& CONSOLE_LOGGER("console");

/**
 * Parse and calculate a VWAP for a binary NASDAQ ITCH 5.0 file
 */
int main(int argc, char* argv[]) {

    // Initialize the console logger
    auto consoleLogger = spdlog::stdout_color_mt(CONSOLE_LOGGER);

    std::string filepath("../../ITCHFiles/01302019.NASDAQ_ITCH50");
    
    // Open File    
    std::ifstream file(filepath, std::ios::binary);

    if (!file) {
        consoleLogger -> info("Failed to open the file. Quitting...");
        return 1;
    }

    // Start execution timer
    auto start = std::chrono::system_clock::now();

    // Initialize the buffer
    constexpr size_t bufferSize = 64;  // 64 is a pretty ok buffer size. The largest message is 50 bytes, so why not leave a few more for good measure.
    std::vector<char> buffer(bufferSize);

    while(file) {

        // For some reason, there happens to be two leading bytes at the start of each line
        file.read(buffer.data(), NUMBER_OF_BYTES_FOR_HEADER_CHUNK);
        MessageHeader header = parseHeader(&buffer[NUMBER_OF_BYTES_OFFSET_FOR_HEADER_CHUNK]);

        // Get the message body 
        size_t numberOfBytesForBody = messageTypeToNumberOfBytes(header.getMessageType());
        file.read(&buffer[NUMBER_OF_BYTES_FOR_HEADER_CHUNK], numberOfBytesForBody);

    }
    // Close file
    file.close();

    auto end = std::chrono::system_clock::now();
    // Convert nanos to seconds
    consoleLogger -> info("======== Total program execution time: {} seconds ========", static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(end - start).count()));

    return 0;
}
