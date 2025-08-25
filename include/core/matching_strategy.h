#pragma once
#include "../common/message.h"
#include "../common/types.h"
#include <vector>
#include <map>
#include <deque>
#include <functional>

class MatchingStrategy {
public:
    virtual std::vector<Event> match(
        const SystemProtocol& newOrder,
        // greater sorts the map from highest to lowest
        std::map<uint64_t, std::deque<SystemProtocol>, std::greater<uint64_t>>& bids,
        std::map<uint64_t, std::deque<SystemProtocol>>& asks
    ) = 0;
    virtual ~MatchingStrategy() = default;
};