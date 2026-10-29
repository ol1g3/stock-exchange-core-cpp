#pragma once
#include "service_interface.h"
#include "snapshot_service.h"
#include "retransmission_service.h"
#include "../core/order_book.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class OrderEntryGateway : public ServiceInterface {
private:
    std::atomic<bool> running;
    OrderBookPool& orderBookPool;
    BatchSystemProtocol batch;
    std::chrono::milliseconds batchInterval;
    std::mutex batchMutex;
    std::condition_variable batchCv;
    std::thread batchThread;

    bool run();

public:
    OrderEntryGateway(OrderBookPool& orderBookPool,
                      std::chrono::milliseconds batchInterval = std::chrono::milliseconds(2000));
    bool acceptOrder(const ClientProtocol& message);

    ServiceType initialize();
    bool start() override;
    bool stop() override;
    bool isRunning() override;

    void process(const BatchSystemProtocol& batch);
    bool forward(const SystemProtocol& message);
    ServiceType getType() const override;
    
    ~OrderEntryGateway() override = default;
};