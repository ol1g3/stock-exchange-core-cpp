#pragma once
#include <string>

enum struct EventType {
    COMPLETED = 1,
    UPDATED = 2,
};

struct Event {
    uint64_t timestamp;
    uint64_t orderId;
    uint8_t type;

    Event() : timestamp(0), orderId(0), type(0) {}
    Event(uint64_t timestamp, uint64_t orderId, uint8_t type) : timestamp(timestamp), orderId(orderId), type(type) {}
};