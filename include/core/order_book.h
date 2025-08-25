#pragma once
#include <vector>
#include <map>
#include <deque>
#include "../common/message.h"
#include "../common/types.h"
#include "../queue/event_queue.h"
#include "../core/fifo_strategy.h"

class OrderBook {
private:
    EventQueue eventQueue;
    std::map<uint64_t, std::deque<SystemProtocol> > bids;
    std::map<uint64_t, std::deque<SystemProtocol> > asks;
    FIFOStrategy strategy;

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