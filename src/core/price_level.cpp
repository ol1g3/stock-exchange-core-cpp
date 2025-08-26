#include "../../include/core/price_level.h"

bool PriceLevel::empty() const {
    return size() == 0;
}

size_t PriceLevel::size() const {
    return ordersBySize.size() + ordersByTime.size();
}

void PriceLevel::addOrder(const SystemProtocol& order) {
    if (comparisonType == OrderComparisonType::SIZE) {
        ordersBySize.insert(order);
    }
    else {
        ordersByTime.insert(order);
    }
}

void PriceLevel::removeOrder(const SystemProtocol& order) {
    if (comparisonType == OrderComparisonType::SIZE && ordersBySize.find(order) != ordersBySize.end()) {
        ordersBySize.erase(order);
    }
    else if (comparisonType == OrderComparisonType::TIME && ordersByTime.find(order) != ordersByTime.end()) {
        ordersByTime.erase(order);
    }
}

void PriceLevel::updateOrder(const SystemProtocol& order) {
    removeOrder(order);
    addOrder(order);
}

SystemProtocol PriceLevel::getBestOrder() const {
    if (comparisonType == OrderComparisonType::SIZE) {
        if (!ordersBySize.empty()) {
            return *ordersBySize.begin();
        }
    }
    else {
        if (!ordersByTime.empty()) {
            return *ordersByTime.begin();
        }
    }
    
    return SystemProtocol();
}

std::vector<SystemProtocol> PriceLevel::getAllOrders() const {
    std::vector<SystemProtocol> result;
    result.reserve(ordersBySize.size() + ordersByTime.size());

    for (const auto& order : ordersBySize) result.push_back(order);
    for (const auto& order : ordersByTime) result.push_back(order);

    return result;
}
