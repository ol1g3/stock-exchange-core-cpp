#pragma once
#include "matching_strategy.h"

class ProRataStrategy : public MatchingStrategy {
public:
    std::vector<Event> match(
        SystemProtocol& newOrder,
        std::map<uint64_t, PriceLevel, std::greater<uint64_t>>& bids,
        std::map<uint64_t, PriceLevel>& asks
    ) override;

    OrderComparisonType levelType() const override;

    ~ProRataStrategy() override = default;

private:
    int16_t allocate(std::vector<Event>& events, PriceLevel& level, int16_t incoming);
};
