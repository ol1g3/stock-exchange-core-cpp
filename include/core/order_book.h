#pragma once
#include <vector>
#include <map>
#include <deque>
#include <memory>
#include <functional>
#include <unordered_map>
#include "../common/message.h"
#include "../common/types.h"
#include "../queue/event_queue.h"
#include "../core/matching_strategy.h"
#include "../core/price_level.h"

class OrderBook {
private:
    EventQueue& eventQueue;
    std::unique_ptr<MatchingStrategy> strategy;
    std::map<uint64_t, PriceLevel, std::greater<uint64_t>> bids;
    std::map<uint64_t, PriceLevel> asks;

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
    // partition by userId (for now)
    std::unordered_map<uint32_t, uint32_t> instrumentToBookIndex;
public:
    OrderBookPool() = default;
    OrderBookPool(int n, EventQueue& eventQueue);
    bool add(OrderBook&& orderBook);
    OrderBook* get(int ind);
    bool process(const SystemProtocol& order);
    ~OrderBookPool() = default;
};