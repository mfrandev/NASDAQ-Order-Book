#ifndef PROCESS_SYSTEM_EVENT_
#define PROCESS_SYSTEM_EVENT_

#include <cstdint>

#include <BinaryMessageWrapper.h>
#include <SystemEvent.hpp>
#include <SystemEvents.h>
#include <SystemEventSequence.h>


void setEventSequence(SystemEventSequence& systemEventSequence, char eventCode, uint64_t seqNumber) {
    switch(eventCode) 
    {
        case 'O': // SystemEventCodes::StartOfMessages:
            systemEventSequence.startOfMessagesSeq.store(seqNumber, std::memory_order_release);
            break;
        case 'S': // SystemEventCodes::StartOfSystemHours:
            systemEventSequence.startOfSystemHoursSeq.store(seqNumber, std::memory_order_release);
            break;
        case 'Q': // SystemEventCodes::StartOfMarketHours:
            systemEventSequence.startOfMarketHoursSeq.store(seqNumber, std::memory_order_release);
            break;
        case 'M': // SystemEventCodes::EndOfMarketHours:
            systemEventSequence.endOfMarketHoursSeq.store(seqNumber, std::memory_order_release);
            break;
        case 'E': // SystemEventCodes::EndOfSystemHours:
            systemEventSequence.endOfSystemHoursSeq.store(seqNumber, std::memory_order_release);
            break;
        case 'C': // SystemEventCodes::EndOfMessages:
            systemEventSequence.endOfMessagesSeq.store(seqNumber, std::memory_order_release);
            break;
    }
}

void processSystemEvent(BinaryMessageWrapper&& msg, SystemEventSequence& systemEventSequence) { 
    SystemEvent systemEvent = parseSystemEventBody(msg.buffer);
    setEventSequence(systemEventSequence, systemEvent.eventCode, msg.seq);
}       

#endif // PROCESS_SYSTEM_EVENT_