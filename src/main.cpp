#include <fstream>
#include <vector>
#include <chrono>
#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <MessageHeader.hpp>
#include <BinaryMessageWrapper.h>

#include <ShardManager.hpp>

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

    constexpr int readerThreadCore = 2;
    if(!pin_to_core(readerThreadCore)) {
        consoleLogger -> info("Failed to pin reader thread to core {}", readerThreadCore);
    }

    // Start execution timer
    auto start = std::chrono::system_clock::now();

    {
        ShardManager<NUMBER_OF_SHARDS, SHARD_SIZE> shardManager; // Default constructed, class itself knows all capacity values
        shardManager.start();
        // uint32_t counter = 0;
        uint64_t messageSequenceCounter = 0;
        while(file) {

            BinaryMessageWrapper wrappedMessage;

            wrappedMessage.seq = messageSequenceCounter;
            // For some reason, there happens to be two leading bytes at the start of each line
            file.read(wrappedMessage.buffer, NUMBER_OF_BYTES_FOR_HEADER_CHUNK);
            wrappedMessage.header = parseHeader(&wrappedMessage.buffer[NUMBER_OF_BYTES_OFFSET_FOR_HEADER_CHUNK]);
            // consoleLogger -> info ("{}. {} {}", wrappedMessage.header.messageType, wrappedMessage.header.stockLocate, wrappedMessage.header.timestamp); 
            file.read(wrappedMessage.buffer, messageTypeToNumberOfBytes(wrappedMessage.header.messageType));
            // Now ready to add to queue

            shardManager.dispatch(std::move(wrappedMessage));
            // ++counter;
            ++messageSequenceCounter;
        }
        // Close file
        file.close();
        // consoleLogger->info("Reader dispatched {} messages", counter);
        auto end = std::chrono::system_clock::now();
        // Convert nanos to seconds
        consoleLogger -> info("======== Total Order Book Management Time: {} seconds ========", static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(end - start).count()));
        consoleLogger->info("Joining Consumers..."); 
        shardManager.shutDownConsumers();    
        consoleLogger->info("Consumers joined");  
        
        std::vector<std::string> query = {"AAPL", "XOM"};
        auto result = shardManager.queryDayAndLastOneAndLastFiveMintues(query);
        for(int i = 0; i < query.size(); ++i) {
            std::cout << "Symbol: " << query[i] << " One: " << result[i][0] << " Five: " << result[i][1] << " Day: " << result[i][2] << std::endl;
        }
        // std::cout << "===== Final Order Book Dump =====" << std::endl;
        // shardManager.dumpOrderBooks(std::cout);
    }

    return 0;
}
