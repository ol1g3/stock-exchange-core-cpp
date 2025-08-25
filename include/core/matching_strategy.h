#include "../common/message.h"
#include "../common/types.h"
#include <vector>
#include <map>
#include <deque>

class MatchingStrategy {
public:
    virtual std::vector<Event> match(
        const SystemProtocol& newOrder,
        std::map<uint64_t, std::deque<SystemProtocol> >& bids,
        std::map<uint64_t, std::deque<SystemProtocol> >& asks
    ) = 0;
    virtual ~MatchingStrategy() = default;
};