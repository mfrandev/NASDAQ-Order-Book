#include <SystemEvent.h>

#include <spdlog/spdlog.h>

#include <time_utils.h>
#include <VWAPManager.h>

extern bool workFinished;
extern uint64_t marketCloseTimestamp;
extern const std::string& CONSOLE_LOGGER;

bool SystemEvent::processMessage() const {
    switch(eventCode) {
        case EVENT_CODE_END_OF_MARKET_HOURS:
            closeMarket(header.getTimestamp());
            spdlog::get(CONSOLE_LOGGER) -> info("Market closed at {}", header.getTimestamp());
            break;
    }
    return true;
}

/**
 * Parse the system event body
 */
SystemEvent* parseSystemEventBody(BinaryMessageHeader header, const char* data) {
    size_t offset = 0;
    char eventCode = data[offset];
    return new SystemEvent(std::move(header), eventCode);
}
