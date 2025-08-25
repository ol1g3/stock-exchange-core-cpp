#include "../../include/core/fifo_strategy.h"
#include <chrono>

std::vector<Event> FIFOStrategy::match(
        const SystemProtocol& newOrder,
        std::map<uint64_t, std::deque<SystemProtocol>>& bids,
        std::map<uint64_t, std::deque<SystemProtocol>>& asks
    ) {
    
    std::vector<Event> events;
    SystemProtocol order = newOrder;

    if (order.side) {
        for (auto it = asks.begin(); it != asks.end() && order.quantity > 0; ) {
            if (it->first > order.price) {
                break;
            }

            auto& priceLevelQueue = it->second;
            while (!priceLevelQueue.empty() && order.quantity > 0) {
                SystemProtocol& restingOrder = priceLevelQueue.front();
                int minVal = std::min(order.quantity, restingOrder.quantity);
                order.quantity -= minVal;
                restingOrder.quantity -= minVal;
                
                uint64_t now = std::chrono::system_clock::now().time_since_epoch().count();
                events.push_back({now, static_cast<uint8_t>(EventType::COMPLETED)});
                events.push_back({now, static_cast<uint8_t>(EventType::COMPLETED)});

                if (restingOrder.quantity == 0) {
                    priceLevelQueue.pop_front();
                }
            }

            if (priceLevelQueue.empty()) {
                it = asks.erase(it);
            }
            else it ++;
        }
        if (order.quantity > 0) {
            bids[order.price].push_back(order);
        }
    }
    else {
        for (auto it = bids.begin(); it != bids.end() && order.quantity > 0; ) {
            if (it->first < order.price) {
                break;
            }

            auto& priceLevelQueue = it->second;
            while (!priceLevelQueue.empty() && order.quantity > 0) {
                SystemProtocol& restingOrder = priceLevelQueue.front();
                int minVal = std::min(order.quantity, restingOrder.quantity);

                order.quantity -= minVal;
                restingOrder.quantity -= minVal;

                uint64_t now = std::chrono::system_clock::now().time_since_epoch().count();
                events.push_back({now, static_cast<uint8_t>(EventType::COMPLETED)});
                events.push_back({now, static_cast<uint8_t>(EventType::COMPLETED)});

                if (restingOrder.quantity == 0) {
                    priceLevelQueue.pop_front();
                }
            }

            if (priceLevelQueue.empty()) {
                it = bids.erase(it);
            }
            else it ++;
        }
        if (order.quantity > 0) {
            asks[order.price].push_back(order);
        }
    }
    return events;
}
