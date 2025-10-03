#pragma once
#include <vector>
#include <map>
#include <unordered_map>
#include "../common/message.h"
#include "../queue/event_queue.h"
#include "../core/matching_strategy.h"
#include "../core/price_level.h"
#include "../services/retransmission_service.h"

class OrderBook {
private:
    EventQueue& eventQueue;
    std::unique_ptr<MatchingStrategy> strategy;
    std::map<uint64_t, PriceLevel, std::greater<uint64_t>> bids;
    std::map<uint64_t, PriceLevel> asks;
    static inline std::atomic<int> expectedTransactionNum{0};
    static inline std::atomic<int> lastProcessedSeqNum{1};
public:
    OrderBook(EventQueue& eventQueue, std::unique_ptr<MatchingStrategy> strategy);
    bool process(const BatchSystemProtocol& message);
    uint64_t getLowestSell();
    uint64_t getHighestBuy();
    
    OrderBook(OrderBook&&) = delete;
    OrderBook(const OrderBook&) = delete;
    OrderBook& operator=(OrderBook&&) = delete;
    OrderBook& operator=(const OrderBook&) = delete;
    
    std::vector<SystemProtocol> requestRetransmission(const int& fromId, const int& toId);
    std::vector<BatchSystemProtocol> requestSnapshots(const int& fromSeqNum, const int& toSeqNum);

    std::vector<std::pair<uint64_t, int16_t>> getTopPriceLevels(bool isBid, int depth) const;
    ~OrderBook() = default;
};

class OrderBookPool {
private:
    std::vector<std::unique_ptr<OrderBook>> pool;
    std::unordered_map<uint32_t, uint32_t> instrumentToBookIndex;
public:
    OrderBookPool() = default;
    OrderBookPool(int n, EventQueue& eventQueue);
    bool add(std::unique_ptr<OrderBook> orderBook);
    size_t getSize() const;
    OrderBook* get(size_t ind);
    bool process(const BatchSystemProtocol& order);
    ~OrderBookPool() = default;
};