#include "../../include/core/matching_strategy.h"
#include <chrono>

std::vector<Event> MatchingStrategy::batchedMatch(
        std::vector<SystemProtocol>& newBatch,
        std::map<uint64_t, PriceLevel, std::greater<uint64_t>>& bids,
        std::map<uint64_t, PriceLevel>& asks
    ) {
    std::vector<Event> events;
    for (SystemProtocol order : newBatch) {
        if (validateOrder(order)) {
            std::vector<Event> newEvents = match(order, bids, asks);
            events.insert(std::end(events), newEvents.begin(), newEvents.end());
        }
    }
    return events;
}

bool MatchingStrategy::validateOrder(const SystemProtocol& order) const {
    if (order.price <= 0) return false;
    if (order.quantity <= 0) return false;
    if (order.userId <= 0) return false;
    if (order.quantity > MAX_QUANTITY) return false;
    
    return true;
}

void MatchingStrategy::processFilledOrder(
    std::vector<Event>& events, 
    const SystemProtocol& currentOrder,
    PriceLevel& priceLevel
) {
    uint64_t now = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
        
    events.push_back({now, currentOrder.transaction_id, static_cast<uint8_t>(OrderStatus::Filled)});
    priceLevel.removeOrder(currentOrder);
}

void MatchingStrategy::processPartiallyFilledOrder(
    std::vector<Event>& events,
    const SystemProtocol& currentOrder,
    SystemProtocol& updatedOrder,
    PriceLevel& priceLevel
) {
    uint64_t now = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
        
    events.push_back({now, currentOrder.transaction_id, static_cast<uint8_t>(OrderStatus::PartiallyFilled)});
    priceLevel.updateOrder(updatedOrder);
}