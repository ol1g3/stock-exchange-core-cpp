#pragma once
#include <deque>
#include <set>
#include <unordered_map>
#include <memory>
#include <optional>
#include "../common/message.h"

enum struct OrderComparisonType {
    TIME,
    SIZE,
};

struct OrderSizeComparator {
    bool operator()(const SystemProtocol& a, const SystemProtocol& b) const {
        if(a.quantity != b.quantity) return a.quantity > b.quantity;
        return a.timestamp < b.timestamp;
    }
};

struct OrderTimeComparator {
    bool operator()(const SystemProtocol& a, const SystemProtocol& b) const {
        return a.timestamp < b.timestamp;
    }
};

class PriceLevel {
private:
    OrderComparisonType comparisonType;
    
    std::set<SystemProtocol, OrderTimeComparator> ordersByTime;
    std::set<SystemProtocol, OrderSizeComparator> ordersBySize;
    
public:
    PriceLevel() : comparisonType(OrderComparisonType::TIME) {}
    PriceLevel(OrderComparisonType type) : comparisonType(type) {}
    
    bool empty() const;
    void addOrder(const SystemProtocol& order);
    void removeOrder(const SystemProtocol& order);
    void updateOrder(const SystemProtocol& order);
    
    SystemProtocol getBestOrder() const;
    
    size_t size() const;
    std::vector<SystemProtocol> getAllOrders() const;
    
    ~PriceLevel() = default;
};