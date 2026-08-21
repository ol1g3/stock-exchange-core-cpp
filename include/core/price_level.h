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
        if(a.timestamp != b.timestamp) return a.timestamp < b.timestamp;
        return a.transaction_id < b.transaction_id;
    }
};

struct OrderTimeComparator {
    bool operator()(const SystemProtocol& a, const SystemProtocol& b) const {
        if(a.timestamp != b.timestamp) return a.timestamp < b.timestamp;
        return a.transaction_id < b.transaction_id;
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
    void updateOrder(const SystemProtocol& oldOrder, const SystemProtocol& newOrder);
    
    SystemProtocol getBestOrder() const;
    
    size_t size() const;
    std::vector<SystemProtocol> getAllOrders() const;
    
    ~PriceLevel() = default;
};