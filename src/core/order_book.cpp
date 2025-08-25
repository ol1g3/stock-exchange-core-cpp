#include "../../include/common/message.h"
#include "../../include/core/order_book.h"
#include <thread>
#include <chrono>
#include <algorithm>

OrderBook::OrderBook(const EventQueue& eventQueue) : eventQueue(eventQueue) {}

bool OrderBook::process(const SystemProtocol& newOrder) {
    std::vector<Event> events = strategy.match(newOrder,
                            bids,
                            asks);
    for(Event event: events) eventQueue.push(event);
    return true;
}

OrderBookPool::OrderBookPool(int n, EventQueue eventQueue) {
    pool.reserve(n);
    for(int i = 0;i < n;i ++)
        pool.emplace_back(eventQueue, std::make_unique<FIFOStrategy>());
}

bool OrderBookPool::add(OrderBook orderBook){
    pool.push_back(orderBook);
    return true;
}

OrderBook* OrderBookPool::get(int ind){
    if(ind >= pool.size()) return NULL;
    return &(pool.at(ind));
}
