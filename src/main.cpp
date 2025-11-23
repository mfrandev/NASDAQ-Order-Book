#include <fstream>
#include <vector>
#include <chrono>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <MessageHeader.hpp>
#include <BinaryMessageWrapper.h>

#include <message_utils.hpp>
#include <queue_utils.hpp>

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


    while(file) {

        BinaryMessageWrapper wrappedMessage;

        // For some reason, there happens to be two leading bytes at the start of each line
        file.read(wrappedMessage.buffer, NUMBER_OF_BYTES_FOR_HEADER_CHUNK);
        wrappedMessage.header = parseHeader(&wrappedMessage.buffer[NUMBER_OF_BYTES_OFFSET_FOR_HEADER_CHUNK]);
        // consoleLogger -> info ("{}. {} {}", header.messageType, header.stockLocate, header.timestamp); 
        file.read(wrappedMessage.buffer, messageTypeToNumberOfBytes(wrappedMessage.header.messageType));
        // Now ready to add to queue
    }
    // Close file
    file.close();

    auto end = std::chrono::system_clock::now();
    // Convert nanos to seconds
    consoleLogger -> info("======== Total program execution time: {} seconds ========", static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(end - start).count()));

    return 0;
}
