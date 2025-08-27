#pragma once
#include "service_interface.h"
#include "../core/order_book.h"
#include <thread>

class OrderEntryGateway : public ServiceInterface {
private:
    bool running;
    OrderBookPool& orderBookPool;
    BatchSystemProtocol batch;
    std::chrono::milliseconds batchInterval = std::chrono::milliseconds(2000);
    std::mutex batchMutex;
    std::thread batchThread;

public:
    OrderEntryGateway(OrderBookPool& orderBookPool);
    bool acceptOrder(const ClientProtocol& message);

    ServiceType initialize();
    bool start() override;
    bool stop() override;
    bool isRunning() override;
    bool run();
    ServiceType getType() const override;
    
    ~OrderEntryGateway() override = default;
};