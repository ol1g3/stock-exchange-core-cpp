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
    Cancelled = 5,
};

enum struct TimeInForce : uint8_t {
    GTC = 0,
    IOC = 1,
};

struct Event {
    uint64_t timestamp;
    uint64_t orderId;
    OrderStatus status;
    uint32_t userId;
    uint64_t price;
    int16_t quantity;
    bool side;

    Event() : timestamp(0), orderId(0), status(OrderStatus::New), userId(0), price(0), quantity(0), side(true) {}
    Event(uint64_t timestamp, uint64_t orderId, uint8_t status, uint32_t userId, uint64_t price, int16_t quantity, bool side) : timestamp(timestamp), orderId(orderId), status(static_cast<OrderStatus>(status)), userId(userId), price(price), quantity(quantity), side(side) {}
};

struct QuoteSummary {
    uint64_t highestBid;
    uint64_t lowestAsk;
};
