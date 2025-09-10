#pragma once
#include <string>

enum struct EventType {
    COMPLETED = 1,
    UPDATED = 2,
};

enum struct OrderStatus {
    Filled = 1,
    PartiallyFilled = 2,
    Added = 3,
    New = 4,
};

struct Event {
    uint64_t timestamp;
    uint64_t orderId;
    OrderStatus status;

    Event() : timestamp(0), orderId(0), status(OrderStatus::New) {}
    Event(uint64_t timestamp, uint64_t orderId, uint8_t status) : timestamp(timestamp), orderId(orderId), status(static_cast<OrderStatus>(status)) {}
};

struct QuoteSummary {
    uint64_t highestBid;
    uint64_t lowestAsk;
};
