#pragma once
#include "matching_strategy.h"

class FIFOStrategy : public MatchingStrategy {
public:
    std::vector<Event> match(
        const SystemProtocol& newOrder,
        std::map<uint64_t, std::deque<SystemProtocol>, std::greater<uint64_t>>& bids,
        std::map<uint64_t, std::deque<SystemProtocol>>& asks
    ) override;
    ~FIFOStrategy() override = default;
};