#pragma once
#include <string>

enum struct EventType {
    COMPLETED = 1,
    FAILED = 2,
    INFO = 3
};

struct Event {
    uint64_t timestamp;
    uint8_t type;

    Event() : timestamp(0), type(0) {}
    Event(uint64_t timestamp, uint8_t type) : timestamp(timestamp), type(type) {}
};