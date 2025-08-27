#include "../../include/common/message.h"
#include "../../include/core/order_book.h"
#include "../../include/core/fifo_strategy.h"
#include "../../include/services/retransmission_service.h"
#include <chrono>

OrderBook::OrderBook(EventQueue& eventQueue, std::unique_ptr<MatchingStrategy> strategy)
    : eventQueue(eventQueue), strategy(std::move(strategy)) {
}

static int expectedSeqNum = 1;

bool OrderBook::process(const BatchSystemProtocol& batch) {
    if (!strategy) {
        return false;
    }
    std::vector<SystemProtocol> messages = batch.get_all_messages();
    for (size_t i = 0;i < messages.size();i ++) {
        const auto& message = messages.at(i);
        if (message.seq_number != expectedSeqNum) {
            std::vector<SystemProtocol> missingOrders = requestRetransmission(expectedSeqNum, expectedSeqNum + messages.size() - i - 1);
            messages.insert(messages.begin() + i, missingOrders.begin(), missingOrders.end());
        }
        expectedSeqNum ++;
    }
    std::vector<Event> events = strategy->batchedMatch(messages, bids, asks);
    for (const auto& event : events) {
        eventQueue.push(event);
    }
    
    return true;
}

std::vector<SystemProtocol> OrderBook::requestRetransmission(const int &fromId, const int &toId) {
    return RetransmissionService::getInstance().batchRequest(fromId, toId);
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
    return &pool.at(ind);
}

bool OrderBookPool::process(const BatchSystemProtocol& order){
    uint32_t orderBookId = 0; // change later
    // if(instrumentToBookIndex.size() &&  instrumentToBookIndex.find(order.userId) == instrumentToBookIndex.end()) orderBookId = (*instrumentToBookIndex.find(order.userId)).second;
    return pool.at(orderBookId).process(order);
}

int OrderBookPool::getSize() {
    return pool.size();
}