#pragma once
#include "../common/message.h"
#include "../common/types.h"
#include "price_level.h"
#include <vector>
#include <map>
#include <deque>
#include <functional>

class MatchingStrategy {
public:
    virtual std::vector<Event> match(
        SystemProtocol& newOrder,
        // greater sorts the map from highest to lowest
        std::map<uint64_t, PriceLevel, std::greater<uint64_t>>& bids,
        std::map<uint64_t, PriceLevel>& asks
    ) = 0;
    virtual std::vector<Event> batchedMatch(
        std::vector<SystemProtocol>& newBatch,
        // greater sorts the map from highest to lowest
        std::map<uint64_t, PriceLevel, std::greater<uint64_t>>& bids,
        std::map<uint64_t, PriceLevel>& asks
    ) = 0;
    virtual ~MatchingStrategy() = default;
};