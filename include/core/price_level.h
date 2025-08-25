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

struct OrderPointerSizeComparator {
    bool operator()(const SystemProtocol* a, const SystemProtocol* b) const {
        if(a->quantity != b->quantity) return a->quantity > b->quantity;
        return a->timestamp < b->timestamp;
    }
};

struct OrderPointerTimeComparator {
    bool operator()(const SystemProtocol* a, const SystemProtocol* b) const {
        return a->timestamp < b->timestamp;
    }
};

class PriceLevel {
private:
    OrderComparisonType comparisonType;
    std::set<SystemProtocol*, OrderPointerTimeComparator> ordersByTime;
    std::set<SystemProtocol*, OrderPointerSizeComparator> ordersBySize;
public:
    PriceLevel() : comparisonType(OrderComparisonType::TIME) {}
    PriceLevel(OrderComparisonType type) : comparisonType(type) {}
    bool empty();
    void addOrder(SystemProtocol order);
    void removeOrder(SystemProtocol order);
    void updateOrder(SystemProtocol order);

    const SystemProtocol* getBestOrder();

    ~PriceLevel() = default;
};