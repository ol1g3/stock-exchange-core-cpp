#pragma once
#include "../common/message.h"
#include "../common/types.h"
#include "price_level.h"
#include <vector>
#include <map>
#include <deque>
#include <functional>

//! @brief Abstract interface for order matching algorithms used by the exchange.
class MatchingStrategy {
public:
    virtual std::vector<Event> match(
        SystemProtocol& newOrder,
        // greater sorts the map from highest to lowest
        std::map<uint64_t, PriceLevel, std::greater<uint64_t>>& bids,
        std::map<uint64_t, PriceLevel>& asks
    ) = 0;
    
    // Default implementation for batch processing
    virtual std::vector<Event> batchedMatch(
        std::vector<SystemProtocol>& newBatch,
        // greater sorts the map from highest to lowest
        std::map<uint64_t, PriceLevel, std::greater<uint64_t>>& bids,
        std::map<uint64_t, PriceLevel>& asks
    );
    
    virtual ~MatchingStrategy() = default;

protected:
    static const int16_t MAX_QUANTITY = 30000;
    // Helper methods for validation and order processing
    bool validateOrder(const SystemProtocol& order) const;
    
    void processFilledOrder(
        std::vector<Event>& events, 
        const SystemProtocol& currentOrder,
        PriceLevel& priceLevel
    );
    
    void processPartiallyFilledOrder(
        std::vector<Event>& events,
        const SystemProtocol& currentOrder,
        SystemProtocol& updatedOrder,
        PriceLevel& priceLevel
    );
};