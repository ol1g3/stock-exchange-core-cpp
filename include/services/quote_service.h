#pragma once
#include "service_interface.h"
#include "../core/order_book.h"
#include "../client/client.h"
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

class Client;

/**
 * @brief Service for publishing market data (quotes) to clients.
 */
class QuoteService : public ServiceInterface {
private:
      OrderBookPool* orderBookPool;
      std::vector<Client*> subscribers;
      std::thread publishThread;
      std::mutex subscriberMutex;
      std::atomic<bool> running{false};
      std::chrono::milliseconds publishInterval;
      
      void run();
public:
    QuoteService(OrderBookPool* pool, int publishIntervalMs = 100);

    bool start() override;
    bool stop() override;
    bool isRunning() override;
    ServiceType getType() const override;

    // Subscription management
    void subscribe(Client* client);
    void unsubscribe(Client* client);

    uint64_t getHighestBid();
    uint64_t getLowestAsk();
    uint64_t getSpread();

    void publishQuotes();

    // On-demand data
    std::vector<std::pair<uint64_t, int16_t>> getTopBids(int depth = 5);
    std::vector<std::pair<uint64_t, int16_t>> getTopAsks(int depth = 5);

    ~QuoteService() override = default;
};