#include "../../include/common/message.h"
#include "../../include/core/order_book.h"
#include "../../include/core/fifo_strategy.h"
#include <utility>
#include <thread>
#include <chrono>
#include <algorithm>

OrderBook::OrderBook(EventQueue& eventQueue, std::unique_ptr<MatchingStrategy> strategy) 
    : eventQueue(eventQueue), strategy(std::move(strategy)) {}

bool OrderBook::process(const SystemProtocol& newOrder) {
    if (!strategy) {
        return false;
    }
    if(!verifyChecksum(newOrder)) {
        
        // request retransmission service
    }
    std::vector<Event> events = strategy->match(newOrder, bids, asks);
    for (const auto& event : events) {
        eventQueue.push(event);
    }
    
    return true;
}

OrderBookPool::OrderBookPool(int n, EventQueue& eventQueue) {
    pool.reserve(n);
    for(int i = 0; i < n; ++i) {
        pool.emplace_back(eventQueue, std::make_unique<FIFOStrategy>());
    }
}

bool OrderBookPool::add(OrderBook&& orderBook){
    pool.push_back(std::move(orderBook));
    return true;
}

OrderBook* OrderBookPool::get(int ind){
    if(ind < 0 || static_cast<size_t>(ind) >= pool.size()) return nullptr;
    return &pool[ind];
}
