#include "../../include/common/message.h"
#include "../../include/core/order_book.h"
#include <thread>
#include <chrono>
#include <algorithm>

OrderBook::OrderBook(const EventQueue& eventQueue) : eventQueue(eventQueue) {}

bool OrderBook::process(const SystemProtocol& message) {
    // Sleep for 500ms
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    Event event(std::chrono::system_clock::now().time_since_epoch().count(), 
                static_cast<uint8_t>(EventType::COMPLETED));

    
    return eventQueue.push(event); // work with the boolean return here later
}

OrderBookPool::OrderBookPool(int n, EventQueue eventQueue) {
    pool = std::vector<OrderBook>();
    while (--n){
        pool.push_back(*new OrderBook(eventQueue));
    }
}

bool OrderBookPool::add(OrderBook orderBook){
    pool.push_back(orderBook);
    return true;
}

OrderBook* OrderBookPool::get(int ind){
    if(ind >= pool.size()) return NULL;
    return &(pool.at(ind));
}
