#pragma once
#include "matching_strategy.h"

class FIFOStrategy : public MatchingStrategy {
public:
    std::vector<Event> match(
        SystemProtocol& newOrder,
        std::map<uint64_t, PriceLevel, std::greater<uint64_t>>& bids,
        std::map<uint64_t, PriceLevel>& asks
    ) override;

    ~FIFOStrategy() override = default;
};