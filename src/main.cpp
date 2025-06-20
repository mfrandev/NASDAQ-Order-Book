#include <fstream>
#include <vector>
#include <chrono>
#include <string>
#include <thread>
#include <memory>

#include <CLI11.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <ProcessMessage.h>
#include <MessageHeader.h>
#include <SentinelMessage.h>
#include <VWAPManager.h>

#include <queue_utils.h>
#include <time_utils.h>

enum Mode {
    SEQ = 0, 
    LOCKING_SPSC = 1,
    BOOST_LF_SPSC = 2,
    ES_LF_SPSC = 3
};

const std::string& SEQ_STR("seq");
const std::string& LOCKING_SPSC_STR("locking_spsc");
const std::string& BOOST_LF_SPSC_STR("boost_lf_spsc");
const std::string& ES_LF_SPSC_STR("es_lf_spsc");

const std::string& CONSOLE_LOGGER("console");

Mode getModeFromString(const std::string& mode) {
    if(mode == SEQ_STR) return Mode::SEQ;
    else if(mode == LOCKING_SPSC_STR) return Mode::LOCKING_SPSC;
    else if(mode == BOOST_LF_SPSC_STR) return Mode::BOOST_LF_SPSC;
    else if(mode == ES_LF_SPSC_STR) return Mode::ES_LF_SPSC;
    return Mode::BOOST_LF_SPSC;
}

/**
 * Parse and calculate a VWAP for a binary NASDAQ ITCH 5.0 file
 */
int main(int argc, char* argv[]) {

    // Initialize the console logger
    auto consoleLogger = spdlog::stdout_color_mt(CONSOLE_LOGGER);

    std::string filepath("../../ITCHFiles/01302019.NASDAQ_ITCH50");
    std::string mode("lf_spsc");

    CLI::App app{"Nasdaq-Order-Book"};
    app.add_option("-f,--file", filepath, "Input ITCH file");
    app.add_option("-m,--mode", mode, "Execution mode options: [seq, locking_spsc, boost_lf_spsc, es_lf_spsc]");

    CLI11_PARSE(app, argc, argv);

    Mode modeAsEnum = getModeFromString(mode);

    consoleLogger -> info("Using file: {} in mode: {}", filepath, mode);

    // Open File    
    std::ifstream file(filepath, std::ios::binary);

    if (!file) {
        consoleLogger -> info("Failed to open the file. Quitting...");
        return 1;
    }

    // Start execution timer
    auto start = std::chrono::system_clock::now();

    // Initialize the buffer
    constexpr std::size_t bufferSize = 64;  // 64 is a pretty ok buffer size. The largest message is 50 bytes, so why not leave a few more for good measure.
    std::vector<char> buffer(bufferSize);

    bool moreMessagesIncoming = true;
    if(modeAsEnum == Mode::BOOST_LF_SPSC) {
        BoostLockfreeSPSC& mq = BoostLockfreeSPSC::getInstance();

        extern bool workFinished;
        // uint64_t consumerCounter = 0;
        // uint64_t throwawayCounter = 0;
        // Launch the "ITCH Messages" Queue processing thread
        std::thread itchConsumer([&mq, &moreMessagesIncoming, &consoleLogger/*, &consumerCounter, &throwawayCounter*/] {
            uint64_t previousTimestamp = 0;
            // uint64_t counter = 0;
            while(moreMessagesIncoming) {
                if (mq.read_available() == 0) {
                    std::this_thread::yield();
                    continue;
                }

                Message* message = nullptr;
                bool success = mq.popMesageFromLockfreeSPSCQueue(message);
                assert(success); // If this fails, we lost sequentiality invariant
                // consumerCounter++;
            
                if(message == nullptr) continue;

                uint64_t currentTimestamp = message -> getHeader().getTimestamp();
                // Bookkeep time
                ProcessMessage::processHeaderTimestamp(currentTimestamp);
                // consoleLogger -> info("current: {}, prev: {}", currentTimestamp, previousTimestamp);
                // assert(previousTimestamp <= currentTimestamp || previousTimestamp == 0); // Similarly, we are processing messages out of order if this is ever false
                previousTimestamp = currentTimestamp;
                
                // Do bookkeeping, etc.
                bool shouldContinue = message -> processMessage();
                // if(!processed) throwawayCounter++;
                delete message;
                if(!shouldContinue) moreMessagesIncoming = false;
            }
            consoleLogger -> info("Exiting the consumer thread");
        });

        // And here is the magic
        // uint64_t counter = 0;
        // uint64_t counterToQueue = 0;
        while(file) {

            // For some reason, there happens to be two leading bytes at the start of each line
            file.read(buffer.data(), NUMBER_OF_BYTES_FOR_HEADER_CHUNK);
            BinaryMessageHeader header = parseHeader(&buffer[NUMBER_OF_BYTES_OFFSET_FOR_HEADER_CHUNK]);
            // consoleLogger -> info("{}. {} {} /*{} */{}", ++counter, header.getMessageType(), header.getStockLocate(), /*header.getTrackingNumber(),*/ header.getTimestamp());

            // Get the message body 
            std::size_t numberOfBytesForBody = ProcessMessage::messageTypeToNumberOfBytes(header.getMessageType());
            file.read(&buffer[NUMBER_OF_BYTES_FOR_HEADER_CHUNK], numberOfBytesForBody);

            while(mq.write_available() == 0) {
                std::this_thread::yield();
            }

            // Parse the binary message, then add to the processing queue (if not nullptr)
            Message* messagePtr = ProcessMessage::getMessage(&buffer[NUMBER_OF_BYTES_FOR_HEADER_CHUNK], numberOfBytesForBody, std::move(header));
            if(messagePtr == nullptr) {
                delete messagePtr;
                continue;
            }

            bool success = mq.pushMesageToLockfreeSPSCQueue(messagePtr);
            assert(success); // If this fails, we lost sequentiality invariant
            // counterToQueue++;
        }
        // Close file
        file.close();
        // consoleLogger -> info("Processed {} messages, {} were passed to the queue and {} were consumed. {} were nullptr or afterhours for a total of {} fully processed", counter, counterToQueue, consumerCounter, throwawayCounter, consumerCounter - throwawayCounter);
        workFinished = true;
        mq.pushMesageToLockfreeSPSCQueue(new SentinelMessage());

        // Synchronize the producer (main) and consumer threads.
        consoleLogger -> info("Waiting on consumer thread to join... Is work done? {}", isWorkFinished());
        if(itchConsumer.joinable()) itchConsumer.join();
        consoleLogger -> info("Consumer thread joined.");

        // Produce the output
        VWAPManager::getInstance().outputBrokenTradeAdjustedVWAP();
    } 
    
    /**
     * SPSC implementation using Erez Strauss's MPMC queue.
     * https://github.com/erez-strauss/lockfree_mpmc_queue/tree/master 
     */
    else if(modeAsEnum == Mode::ES_LF_SPSC) {
        es::lockfree::pointer_mpmc_queue<es::lockfree::mpmc_queue, Message> mq {SPSC_QUEUE_CAPACITY};

        extern bool workFinished;
        // uint64_t consumerCounter = 0;
        // uint64_t throwawayCounter = 0;
        // Launch the "ITCH Messages" Queue processing thread
        std::thread itchConsumer([&mq, &moreMessagesIncoming, &consoleLogger/*, &consumerCounter, &throwawayCounter*/] {
            uint64_t previousTimestamp = 0;
            // uint64_t counter = 0;
            while(moreMessagesIncoming) {
                if (mq.size() == 0) {
                    std::this_thread::yield();
                    continue;
                }

                std::unique_ptr<Message> message{};
                bool success = mq.pop(message);
                assert(success); // If this fails, queue operation failed and we need to terminate
                // consoleLogger -> info("B: {} {} {}", message -> getHeader().getMessageType(), message -> getHeader().getStockLocate(), /*message -> getHeader().getTrackingNumber(),*/ message -> getHeader().getTimestamp());
                // consumerCounter++;
            
                if(message == nullptr) continue;

                uint64_t currentTimestamp = message -> getHeader().getTimestamp();
                // Bookkeep time
                ProcessMessage::processHeaderTimestamp(currentTimestamp);
                // consoleLogger -> info("current: {}, prev: {}", currentTimestamp, previousTimestamp);
                // assert(previousTimestamp <= currentTimestamp || previousTimestamp == 0); // Similarly, we are processing messages out of order if this is ever false
                previousTimestamp = currentTimestamp;
                
                // Do bookkeeping, etc.
                bool shouldContinue = message -> processMessage();
                // if(!processed) throwawayCounter++;
                if(!shouldContinue) moreMessagesIncoming = false;
            }
            consoleLogger -> info("Exiting the consumer thread");
        });

        // And here is the magic
        // uint64_t counter = 0;
        // uint64_t counterToQueue = 0;
        while(file) {

            // For some reason, there happens to be two leading bytes at the start of each line
            file.read(buffer.data(), NUMBER_OF_BYTES_FOR_HEADER_CHUNK);
            BinaryMessageHeader header = parseHeader(&buffer[NUMBER_OF_BYTES_OFFSET_FOR_HEADER_CHUNK]);
            // consoleLogger -> info("{}. {} {} /*{} */{}", ++counter, header.getMessageType(), header.getStockLocate(), /*header.getTrackingNumber(),*/ header.getTimestamp());

            // Get the message body 
            std::size_t numberOfBytesForBody = ProcessMessage::messageTypeToNumberOfBytes(header.getMessageType());
            file.read(&buffer[NUMBER_OF_BYTES_FOR_HEADER_CHUNK], numberOfBytesForBody);

            while(mq.size() == SPSC_QUEUE_CAPACITY) {
                std::this_thread::yield();
            }

            // Parse the binary message, then add to the processing queue (if not nullptr)
            std::unique_ptr<Message> messagePtr {ProcessMessage::getMessage(&buffer[NUMBER_OF_BYTES_FOR_HEADER_CHUNK], numberOfBytesForBody, std::move(header))};
            if(messagePtr == nullptr) continue;
            // consoleLogger -> info("A: {} {} {}", messagePtr -> getHeader().getMessageType(), messagePtr -> getHeader().getStockLocate(), /*messagePtr -> getHeader().getTrackingNumber(),*/ messagePtr -> getHeader().getTimestamp());

            bool success = mq.push(std::move(messagePtr));
            assert(success); // If this fails, we queue operation failed
            // counterToQueue++;
        }
        // Close file
        file.close();
        // consoleLogger -> info("Processed {} messages, {} were passed to the queue and {} were consumed. {} were nullptr or afterhours for a total of {} fully processed", counter, counterToQueue, consumerCounter, throwawayCounter, consumerCounter - throwawayCounter);
        workFinished = true;
        mq.push(std::unique_ptr<Message>(new SentinelMessage()));

        // Synchronize the producer (main) and consumer threads.
        consoleLogger -> info("Waiting on consumer thread to join... Is work done? {}", isWorkFinished());
        if(itchConsumer.joinable()) itchConsumer.join();
        consoleLogger -> info("Consumer thread joined.");

        // Produce the output
        VWAPManager::getInstance().outputBrokenTradeAdjustedVWAP();
    } 
    
    /**
     * SPSC using a queue synchronized with std::mutex and std::condition_variable
     */
    else if(modeAsEnum == Mode::LOCKING_SPSC) {
        locking::QueueSPSC<Message, SPSC_QUEUE_CAPACITY>& mq = locking::QueueSPSC<Message, SPSC_QUEUE_CAPACITY>::getInstance();

        extern bool workFinished;
        // uint64_t consumerCounter = 0;
        // uint64_t throwawayCounter = 0;
        // Launch the "ITCH Messages" Queue processing thread
        std::thread itchConsumer([&mq, &moreMessagesIncoming, &consoleLogger/*, &consumerCounter, &throwawayCounter*/] {
            uint64_t previousTimestamp = 0;
            // uint64_t counter = 0;
            while(moreMessagesIncoming) {

                Message* message = mq.threadsafe_spsc_pop();
                // consumerCounter++;
            
                if(message == nullptr) continue;

                uint64_t currentTimestamp = message -> getHeader().getTimestamp();
                // Bookkeep time
                ProcessMessage::processHeaderTimestamp(currentTimestamp);
                // consoleLogger -> info("current: {}, prev: {}", currentTimestamp, previousTimestamp);
                // consoleLogger -> info("{} {} {}", message -> getHeader().getMessageType(), message -> getHeader().getStockLocate(), /*message -> getHeader().getTrackingNumber(),*/ message -> getHeader().getTimestamp());
                // assert(previousTimestamp <= currentTimestamp || previousTimestamp == 0); // Similarly, we are processing messages out of order if this is ever false
                previousTimestamp = currentTimestamp;
                
                // Do bookkeeping, etc.
                bool shouldContinue = message -> processMessage();
                // if(!processed) throwawayCounter++;
                delete message;
                if(!shouldContinue) moreMessagesIncoming = false;
            }
            consoleLogger -> info("Exiting the consumer thread");
        });

        // And here is the magic
        uint64_t counter = 0;
        uint64_t counterToQueue = 0;
        while(file) {

            // For some reason, there happens to be two leading bytes at the start of each line
            file.read(buffer.data(), NUMBER_OF_BYTES_FOR_HEADER_CHUNK);
            BinaryMessageHeader header = parseHeader(&buffer[NUMBER_OF_BYTES_OFFSET_FOR_HEADER_CHUNK]);
            // consoleLogger -> info("{}. {} {} /*{} */{}", ++counter, header.getMessageType(), header.getStockLocate(), /*header.getTrackingNumber(),*/ header.getTimestamp());
            ++counter;
            // Get the message body 
            std::size_t numberOfBytesForBody = ProcessMessage::messageTypeToNumberOfBytes(header.getMessageType());
            file.read(&buffer[NUMBER_OF_BYTES_FOR_HEADER_CHUNK], numberOfBytesForBody);

            // Parse the binary message, then add to the processing queue (if not nullptr)
            Message* messagePtr = ProcessMessage::getMessage(&buffer[NUMBER_OF_BYTES_FOR_HEADER_CHUNK], numberOfBytesForBody, std::move(header));
            if(messagePtr == nullptr) {
                delete messagePtr;
                continue;
            }

            // Thread sync handled in the queue structure itself
            mq.threadsafe_spsc_push(messagePtr);
            counterToQueue++;
        }
        // Close file
        file.close();
        // consoleLogger -> info("Read {} messages, Sent {} messages", counter, counterToQueue);
        workFinished = true;
        mq.threadsafe_spsc_push(new SentinelMessage());

        // Synchronize the producer (main) and consumer threads.
        consoleLogger -> info("Waiting on consumer thread to join... Is work done? {}", isWorkFinished());
        if(itchConsumer.joinable()) itchConsumer.join();
        consoleLogger -> info("Consumer thread joined.");

        // Produce the output
        VWAPManager::getInstance().outputBrokenTradeAdjustedVWAP();
    } 
    
    /**
     * Single threaded mode. This should be the performance baseline.
     */
    else if(modeAsEnum == Mode::SEQ) {
        while(file) {

            // For some reason, there happens to be two leading bytes at the start of each line
            file.read(buffer.data(), NUMBER_OF_BYTES_FOR_HEADER_CHUNK);
            BinaryMessageHeader header = parseHeader(&buffer[NUMBER_OF_BYTES_OFFSET_FOR_HEADER_CHUNK]);
            // consoleLogger -> info("{}. {} {} /*{} */{}", ++counter, header.getMessageType(), header.getStockLocate(), /*header.getTrackingNumber(),*/ header.getTimestamp());

            // Get the message body 
            std::size_t numberOfBytesForBody = ProcessMessage::messageTypeToNumberOfBytes(header.getMessageType());
            file.read(&buffer[NUMBER_OF_BYTES_FOR_HEADER_CHUNK], numberOfBytesForBody);

            // Parse the binary message, then add to the processing queue (if not nullptr)
            Message* messagePtr = ProcessMessage::getMessage(&buffer[NUMBER_OF_BYTES_FOR_HEADER_CHUNK], numberOfBytesForBody, std::move(header));
            if(messagePtr == nullptr) {
                delete messagePtr;
                continue;
            }
            ProcessMessage::processHeaderTimestamp(messagePtr -> getHeader().getTimestamp());
            messagePtr -> processMessage();
            delete messagePtr;
        }
        // Close file
        file.close();

        // Produce the output
        VWAPManager::getInstance().outputBrokenTradeAdjustedVWAP();
    }

    auto end = std::chrono::system_clock::now();
    // Convert nanos to seconds
    consoleLogger -> info("======== Total program execution time: {} seconds ========", static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(end - start).count()));

    return 0;
}