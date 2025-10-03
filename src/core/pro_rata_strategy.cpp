#include "../../include/core/pro_rata_strategy.h"
#include "../../include/common/types.h"
#include <algorithm>

std::vector<Event> ProRataStrategy::match(
        SystemProtocol& newOrder,
        std::map<uint64_t, PriceLevel, std::greater<uint64_t>>& bids,
        std::map<uint64_t, PriceLevel>& asks
    ) {
    
    if (!validateOrder(newOrder)) {
        return {};
    }
    
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
                int16_t minVal = std::min(order.quantity, cur.quantity);

                order.quantity -= minVal;
                
                SystemProtocol updatedOrder = cur;
                updatedOrder.quantity -= minVal;

                if (updatedOrder.quantity == 0) {
                    processFilledOrder(events, cur, priceLevelQueue);
                }
                else {
                    processPartiallyFilledOrder(events, cur, updatedOrder, priceLevelQueue);
                }
            }

            if (priceLevelQueue.empty()) {
                it = asks.erase(it);
            }
            else it++;
        }
        if (order.quantity > 0) {
            bids[order.price].addOrder(order);
            events.push_back({order.timestamp, order.transaction_id, static_cast<uint8_t>(OrderStatus::New)});
        }
        else {
            events.push_back({order.timestamp, order.transaction_id, static_cast<uint8_t>(OrderStatus::Filled)});
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
                int16_t minVal = std::min(order.quantity, cur.quantity);

                order.quantity -= minVal;
                
                SystemProtocol updatedOrder = cur;
                updatedOrder.quantity -= minVal;

                if (updatedOrder.quantity == 0) {
                    processFilledOrder(events, cur, priceLevelQueue);
                }
                else {
                    processPartiallyFilledOrder(events, cur, updatedOrder, priceLevelQueue);
                }
            }

            if (priceLevelQueue.empty()) {
                it = bids.erase(it);
            }
            else it++;
        }
        if (order.quantity > 0) {
            asks[order.price].addOrder(order);
            events.push_back({order.timestamp, order.transaction_id, static_cast<uint8_t>(OrderStatus::New)});
        }
        else {
            events.push_back({order.timestamp, order.transaction_id, static_cast<uint8_t>(OrderStatus::Filled)});
        }
    }
    return events;
}