#pragma once
#include "service_interface.h"
#include <unordered_set>
#include <mutex>
#include <thread>

class OrderBook;
class OrderBookPool;
class RetransmissionService : public ServiceInterface {
private:
    static RetransmissionService* instance;
    static std::mutex instanceMutex;
    std::thread instanceThread;

    RetransmissionService();

    std::unordered_set<OrderBook*> consumers;
    std::queue<SystemProtocol> orders;
    bool running;

public:
    RetransmissionService(const RetransmissionService&) = delete;
    RetransmissionService& operator=(const RetransmissionService&) = delete;
    static RetransmissionService& getInstance();

    bool start() override;
    bool stop() override;
    bool isRunning() override;
    void runRetransmission();
    ServiceType getType() const override;

    bool addConsumer(OrderBook* orderBook);
    bool addPoolConsumers(OrderBookPool& orderPool);
    std::vector<SystemProtocol> batchRequest(const int& fromId, const int& toId);
    
    ~RetransmissionService() override = default;
};