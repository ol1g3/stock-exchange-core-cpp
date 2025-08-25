#pragma once
#include <vector>
#include <map>
#include <deque>
#include <memory>
#include <functional>
#include "../common/message.h"
#include "../common/types.h"
#include "../queue/event_queue.h"
#include "../core/matching_strategy.h"

class OrderBook {
private:
    EventQueue& eventQueue;
    std::unique_ptr<MatchingStrategy> strategy;
    std::map<uint64_t, std::deque<SystemProtocol>, std::greater<uint64_t>> bids;
    std::map<uint64_t, std::deque<SystemProtocol>> asks;

public:
    OrderBook(EventQueue& eventQueue, std::unique_ptr<MatchingStrategy> strategy);
    bool process(const SystemProtocol& message);
    
    OrderBook(OrderBook&&) = default;

    OrderBook(const OrderBook&);
    OrderBook& operator=(const OrderBook&);
    OrderBook& operator=(OrderBook&&);

    ~OrderBook() = default;
};

class OrderBookPool {
private:
    std::vector<OrderBook> pool;
public:
    OrderBookPool() = default;
    OrderBookPool(int n, EventQueue& eventQueue);
    bool add(OrderBook&& orderBook);
    OrderBook* get(int ind);
    ~OrderBookPool() = default;
};