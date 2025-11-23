#ifndef SYSTEM_EVENT_H_
#define SYSTEM_EVENT_H_

#include <cstddef>

struct SystemEvent {

    char eventCode;

};

SystemEvent parseSystemEventBody(const char* data) {
    size_t offset = 0;
    char eventCode = data[offset];
    return SystemEvent{eventCode};
}

#endif // SYSTEM_EVENT_H_