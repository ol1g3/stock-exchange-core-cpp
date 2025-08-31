#include "../../include/services/quote_service.h"
#include <algorithm>
#include <limits>
#include <set>

QuoteService::QuoteService(OrderBookPool* pool, int publishIntervalMs)
    : orderBookPool(pool), publishInterval(std::chrono::milliseconds(publishIntervalMs)) {
}

bool QuoteService::start() {
    running = true;
    publishThread = std::thread(&QuoteService::run, this);
    return true;
}

bool QuoteService::stop() {
    running = false;
    if(publishThread.joinable()) {
        publishThread.join();
    }
    return true;
}

bool QuoteService::isRunning() {
    return running;
}

ServiceType QuoteService::getType() const {
    return ServiceType::QuoteService;
}

void QuoteService::subscribe(Client* client) {
    std::lock_guard<std::mutex> lock(subscriberMutex);
    if(std::find(subscribers.begin(), subscribers.end(), client) == subscribers.end()) {
        subscribers.push_back(client);
    }
}

void QuoteService::unsubscribe(Client* client) {
    std::lock_guard<std::mutex> lock(subscriberMutex);
    auto it = std::find(subscribers.begin(), subscribers.end(), client);
    if(it != subscribers.end()) {
        subscribers.erase(it);
    }
}

uint64_t QuoteService::getHighestBid() {
    if(!orderBookPool || orderBookPool->getSize() == 0) {
        return 0;
    }
    
    OrderBook* book = orderBookPool->get(0);
    if(!book) {
        return 0;
    }
    
    return 0;
}

uint64_t QuoteService::getLowestAsk() {
    if(!orderBookPool || orderBookPool->getSize() == 0) {
        return std::numeric_limits<uint64_t>::max();
    }
    
    OrderBook* book = orderBookPool->get(0);
    if(!book) {
        return std::numeric_limits<uint64_t>::max();
    }
    
    return 0;
}

uint64_t QuoteService::getSpread() {
    uint64_t highestBid = getHighestBid();
    uint64_t lowestAsk = getLowestAsk();
    if(highestBid == 0 || lowestAsk == std::numeric_limits<uint64_t>::max()) {
        return 0;
    }
    
    return lowestAsk - highestBid;
}

std::vector<std::pair<uint64_t, int16_t>> QuoteService::getTopBids(int depth) {
    if (!orderBookPool || orderBookPool->getSize() == 0) {
        return {};
    }
    
    std::map<uint64_t, int16_t, std::greater<uint64_t>> aggregatedBids;
    for (int i = 0; i < orderBookPool->getSize(); i++) {
        OrderBook* book = orderBookPool->get(i);
        if (!book) continue;
        auto bookLevels = book->getTopPriceLevels(true, depth);
        
        for (const auto& level : bookLevels) {
            aggregatedBids[level.first] += level.second;
        }
        if(aggregatedBids.size() > depth) {
            aggregatedBids.erase(std::prev(aggregatedBids.end()));
        }
    }
    
    std::vector<std::pair<uint64_t, int16_t>> result;
    result.reserve(depth);
    
    int count = 0;
    for (const auto& bid : aggregatedBids) {
        result.push_back(bid);
        count++;
    }
    
    return result;
}

std::vector<std::pair<uint64_t, int16_t>> QuoteService::getTopAsks(int depth) {
    if (!orderBookPool || orderBookPool->getSize() == 0) {
        return {};
    }
    
    std::map<uint64_t, int16_t> aggregatedAsks;
    for (int i = 0; i < orderBookPool->getSize(); i++) {
        OrderBook* book = orderBookPool->get(i);
        if (!book) continue;
        auto bookLevels = book->getTopPriceLevels(false, depth);
        
        for (const auto& level : bookLevels) {
            aggregatedAsks[level.first] += level.second;
        }

        if(aggregatedAsks.size() > depth) {
            aggregatedAsks.erase(std::prev(aggregatedAsks.end()));
        }
    }
    
    std::vector<std::pair<uint64_t, int16_t>> result;
    result.reserve(depth);
    
    int count = 0;
    for (const auto& ask : aggregatedAsks) {
        result.push_back(ask);
        count++;
    }
    
    return result;
}

void QuoteService::publishQuotes() {
    std::lock_guard<std::mutex> lock(subscriberMutex);
    
    if(subscribers.empty() || !orderBookPool) {
        return;
    }
    
    QuoteSummary summary;
    summary.highestBid = getHighestBid();
    summary.lowestAsk = getLowestAsk();
    
    for (Client* client : subscribers) {
        client->receiveQuoteUpdate(summary);
    }
}

void QuoteService::run() {
    while (isRunning()) {
        publishQuotes();
        std::this_thread::sleep_for(publishInterval);
    }
}