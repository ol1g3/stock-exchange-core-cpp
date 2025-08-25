#pragma once
#include <vector>
#include "../common/message.h"
#include "../common/types.h"
#include "../queue/event_queue.h"

class OrderBook {
private:
    EventQueue eventQueue;
public:
    OrderBook(const EventQueue& eventQueue);
    
    bool process(const SystemProtocol& message);
    ~OrderBook() = default;
};

class OrderBookPool {
private:
    std::vector<OrderBook> pool;
public:
    OrderBookPool() = default;
    OrderBookPool(int n, EventQueue eventQueue);
    
    bool add(OrderBook orderBook);
    OrderBook* get(int ind);
    ~OrderBookPool() = default;
};