#pragma once
#include "service_interface.h"
#include <unordered_set>
#include <mutex>
#include <thread>

class OrderBook;
class OrderBookPool;

/**
 * @brief Service for retransmitting lost or missing messages to order books.
 */
class RetransmissionService : public ServiceInterface {
private:
    // Singleton pattern
    RetransmissionService();
    RetransmissionService(const RetransmissionService&) = delete;
    RetransmissionService& operator=(const RetransmissionService&) = delete;

    static RetransmissionService* instance;
    static std::mutex instanceMutex;

    std::thread instanceThread;
    bool running{false};

    std::unordered_set<OrderBook*> consumers;
    std::queue<SystemProtocol> orders;

    void run();

public:
    // Singleton access
    static RetransmissionService& getInstance();

    bool start() override;
    bool stop() override;
    bool isRunning() override;
    ServiceType getType() const override;

    void processBatch(BatchSystemProtocol batch);

    bool addConsumer(OrderBook* orderBook);
    bool addPoolConsumers(OrderBookPool& orderPool);
    std::vector<SystemProtocol> batchRequest(const int& fromId, const int& toId);
    ~RetransmissionService() override = default;
};