#pragma once
#include "matching_strategy.h"
#include "price_level.h"

class ProRataStrategy : public MatchingStrategy {
public:
    std::vector<Event> match(
        const SystemProtocol& newOrder,
        std::map<uint64_t, PriceLevel, std::greater<uint64_t>>& bids,
        std::map<uint64_t, PriceLevel>& asks
    ) override;
    ~ProRataStrategy() override = default;
};