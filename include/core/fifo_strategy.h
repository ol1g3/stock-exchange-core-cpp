#include "matching_strategy.h"

class FIFOStrategy : public MatchingStrategy {
public:
    std::vector<Event> match(
        const SystemProtocol& newOrder,
        std::map<uint64_t, std::deque<SystemProtocol> >& bids,
        std::map<uint64_t, std::deque<SystemProtocol> >& asks
    ) override;
    ~FIFOStrategy() = default;
};