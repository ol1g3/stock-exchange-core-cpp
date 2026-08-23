#include "../../include/core/fifo_strategy.h"
#include "../../include/common/types.h"
#include <algorithm>

std::vector<Event> FIFOStrategy::match(
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

                events.push_back({order.timestamp, order.transaction_id,
                    static_cast<uint8_t>(order.quantity > 0 ? OrderStatus::PartiallyFilled : OrderStatus::Filled),
                    order.userId, it->first, minVal, order.side});

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
            if (static_cast<TimeInForce>(order.timeInForce) == TimeInForce::IOC) {
                events.push_back({order.timestamp, order.transaction_id, static_cast<uint8_t>(OrderStatus::Cancelled), order.userId, order.price, order.quantity, order.side});
            }
            else {
                bids.try_emplace(order.price, levelType()).first->second.addOrder(order);
                events.push_back({order.timestamp, order.transaction_id, static_cast<uint8_t>(OrderStatus::Added), order.userId, order.price, order.quantity, order.side});
            }
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

                events.push_back({order.timestamp, order.transaction_id,
                    static_cast<uint8_t>(order.quantity > 0 ? OrderStatus::PartiallyFilled : OrderStatus::Filled),
                    order.userId, it->first, minVal, order.side});

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
            if (static_cast<TimeInForce>(order.timeInForce) == TimeInForce::IOC) {
                events.push_back({order.timestamp, order.transaction_id, static_cast<uint8_t>(OrderStatus::Cancelled), order.userId, order.price, order.quantity, order.side});
            }
            else {
                asks.try_emplace(order.price, levelType()).first->second.addOrder(order);
                events.push_back({order.timestamp, order.transaction_id, static_cast<uint8_t>(OrderStatus::Added), order.userId, order.price, order.quantity, order.side});
            }
        }
    }
    return events;
}
