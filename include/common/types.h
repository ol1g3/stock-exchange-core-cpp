#pragma once
#include <cstdint>
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
    uint32_t userId;

    Event() : timestamp(0), orderId(0), status(OrderStatus::New), userId(0) {}
    Event(uint64_t timestamp, uint64_t orderId, uint8_t status, uint32_t userId) : timestamp(timestamp), orderId(orderId), status(static_cast<OrderStatus>(status)), userId(userId) {}
};

struct QuoteSummary {
    uint64_t highestBid;
    uint64_t lowestAsk;
};
