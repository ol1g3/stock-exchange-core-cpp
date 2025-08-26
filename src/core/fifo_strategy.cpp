#include "../../include/core/fifo_strategy.h"
#include "../../include/core/price_level.h"
#include <chrono>
#include <algorithm>

std::vector<Event> FIFOStrategy::match(
        const SystemProtocol& newOrder,
        std::map<uint64_t, PriceLevel, std::greater<uint64_t>>& bids,
        std::map<uint64_t, PriceLevel>& asks
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
                const SystemProtocol& cur = priceLevelQueue.getBestOrder();
                int minVal = std::min(order.quantity, cur.quantity);
                order.quantity -= minVal;
                
                SystemProtocol updatedOrder = cur;
                updatedOrder.quantity -= minVal;
                
                uint64_t now = std::chrono::system_clock::now().time_since_epoch().count();

                if (updatedOrder.quantity == 0) {
                    events.push_back({now, cur.transaction_id, static_cast<uint8_t>(EventType::COMPLETED)});
                    priceLevelQueue.removeOrder(cur);
                }
                else {
                    events.push_back({now, cur.transaction_id, static_cast<uint8_t>(EventType::UPDATED)});
                    priceLevelQueue.updateOrder(updatedOrder);
                }
            }

            if (priceLevelQueue.empty()) {
                it = asks.erase(it);
            }
            else it ++;
        }
        if (order.quantity > 0) {
            bids[order.price].addOrder(order);
        }
    }
    else {
        for (auto it = bids.begin(); it != bids.end() && order.quantity > 0; ) {
            if (it->first < order.price) {
                break;
            }

            auto& priceLevelQueue = it->second;
            while (!priceLevelQueue.empty() && order.quantity > 0) {
                const SystemProtocol& cur = priceLevelQueue.getBestOrder();
                int minVal = std::min(order.quantity, cur.quantity);

                order.quantity -= minVal;
                
                SystemProtocol updatedOrder = cur;
                updatedOrder.quantity -= minVal;

                uint64_t now = std::chrono::system_clock::now().time_since_epoch().count();

                if (updatedOrder.quantity == 0) {
                    events.push_back({now, cur.transaction_id, static_cast<uint8_t>(EventType::COMPLETED)});
                    priceLevelQueue.removeOrder(cur);
                }
                else {
                    events.push_back({now, cur.transaction_id, static_cast<uint8_t>(EventType::UPDATED)});
                    priceLevelQueue.updateOrder(updatedOrder);
                }
            }

            if (priceLevelQueue.empty()) {
                it = bids.erase(it);
            }
            else it ++;
        }
        if (order.quantity > 0) {
            asks[order.price].addOrder(order);
        }
    }
    return events;
}
