#include "../../include/core/pro_rata_strategy.h"
#include "../../include/common/types.h"
#include <algorithm>
#include <vector>

OrderComparisonType ProRataStrategy::levelType() const {
    return OrderComparisonType::SIZE;
}

int16_t ProRataStrategy::allocate(std::vector<Event>& events, PriceLevel& level, int16_t incoming) {
    if (incoming <= 0) return 0;

    std::vector<SystemProtocol> resting = level.getAllOrders();
    if (resting.empty()) return 0;

    int32_t total = 0;
    for (const auto& order : resting) total += order.quantity;
    if (total <= 0) return 0;

    if (incoming >= total) {
        for (const auto& order : resting) processFilledOrder(events, order, level);
        return static_cast<int16_t>(total);
    }

    std::vector<int16_t> allocation(resting.size(), 0);
    int32_t handedOut = 0;
    for (size_t i = 0; i < resting.size(); i++) {
        allocation[i] = static_cast<int16_t>(
            static_cast<int64_t>(incoming) * resting[i].quantity / total);
        handedOut += allocation[i];
    }

    for (size_t i = 0; handedOut < incoming; i = (i + 1) % resting.size()) {
        if (allocation[i] < resting[i].quantity) {
            allocation[i]++;
            handedOut++;
        }
    }

    for (size_t i = 0; i < resting.size(); i++) {
        if (allocation[i] == 0) continue;

        SystemProtocol updatedOrder = resting[i];
        updatedOrder.quantity = static_cast<int16_t>(updatedOrder.quantity - allocation[i]);

        if (updatedOrder.quantity == 0) {
            processFilledOrder(events, resting[i], level);
        }
        else {
            processPartiallyFilledOrder(events, resting[i], updatedOrder, level);
        }
    }
    return incoming;
}

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

            int16_t filled = allocate(events, it->second, order.quantity);
            order.quantity = static_cast<int16_t>(order.quantity - filled);
            if (filled > 0) {
                events.push_back({order.timestamp, order.transaction_id,
                    static_cast<uint8_t>(order.quantity > 0 ? OrderStatus::PartiallyFilled : OrderStatus::Filled),
                    order.userId, it->first, filled, order.side});
            }

            if (it->second.empty()) {
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

            int16_t filled = allocate(events, it->second, order.quantity);
            order.quantity = static_cast<int16_t>(order.quantity - filled);
            if (filled > 0) {
                events.push_back({order.timestamp, order.transaction_id,
                    static_cast<uint8_t>(order.quantity > 0 ? OrderStatus::PartiallyFilled : OrderStatus::Filled),
                    order.userId, it->first, filled, order.side});
            }

            if (it->second.empty()) {
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
