#include "../../include/common/message.h"
#include "../../include/core/order_book.h"
#include "../../include/core/fifo_strategy.h"
#include "../../include/services/retransmission_service.h"
#include "../../include/services/snapshot_service.h"
#include <chrono>

OrderBook::OrderBook(EventQueue& eventQueue, std::unique_ptr<MatchingStrategy> strategy)
    : eventQueue(eventQueue), strategy(std::move(strategy)) {
}

static int expectedTransactionNum = 1;
static int lastProcessedSeqNum = 0;

bool OrderBook::process(const BatchSystemProtocol& batch) {
    if (!strategy) {
        return false;
    }
    std::vector<SystemProtocol> messages = batch.get_all_messages();
    if (messages.at(0).seq_number > lastProcessedSeqNum + 1) {
        std::vector<BatchSystemProtocol> lostBatches = requestSnapshots(lastProcessedSeqNum + 1, messages.at(0).seq_number);
        messages.clear();
        for (const auto& batch : lostBatches) {
            for(const auto& order: batch.get_all_messages()) {
                messages.push_back(order);
            }
        }
    }
    else {
        for (size_t i = 0;i < messages.size();i ++) {
            const auto& message = messages.at(i);
            if (message.seq_number != expectedTransactionNum) {
                std::vector<SystemProtocol> missingOrders = requestRetransmission(expectedTransactionNum, expectedTransactionNum + messages.size() - i - 1);
                messages.insert(messages.begin() + i, missingOrders.begin(), missingOrders.end());
            }
            expectedTransactionNum ++;
        }
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

std::vector<BatchSystemProtocol> OrderBook::requestSnapshots(const int &fromSeqNum, const int &toSeqNum) {
    return SnapshotService::getInstance().getSnaphots(fromSeqNum, toSeqNum);
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

uint64_t OrderBook::getLowestSell() {
    if (asks.size()) return (*asks.begin()).first;
    return std::numeric_limits<uint64_t>::max();
}

uint64_t OrderBook::getHighestBuy() {
    if (bids.size()) return (*bids.begin()).first;
    return 0;
}

std::vector<std::pair<uint64_t, int16_t>> OrderBook::getTopPriceLevels(bool isBid, int depth) const {
    std::vector<std::pair<uint64_t, int16_t>> result;
    result.reserve(depth);
    
    if(isBid) {
        int count = 0;
        for (const auto& level : bids) {
            if(count >= depth) break;
            
            int16_t totalQuantity = 0;
            std::vector<SystemProtocol> orders = level.second.getAllOrders();
            for (const auto& order : orders) {
                totalQuantity += order.quantity;
            }
            
            result.push_back({level.first, totalQuantity});
            count++;
        }
    }
    else {
        int count = 0;
        for (const auto& level : asks) {
            if(count >= depth) break;
            
            int16_t totalQuantity = 0;
            std::vector<SystemProtocol> orders = level.second.getAllOrders();
            for (const auto& order : orders) {
                totalQuantity += order.quantity;
            }
            
            result.push_back({level.first, totalQuantity});
            count++;
        }
    }
    
    return result;
}