#pragma once
#include "service_interface.h"
#include "../core/order_book.h"

class OrderEntryGateway : public ServiceInterface {
private:
    bool running;
    OrderBookPool& orderBookPool;

public:
    OrderEntryGateway(OrderBookPool& orderBookPool);
    bool acceptOrder(const ClientProtocol& message);

    ServiceType initialize() override;
    bool start() override;
    bool stop() override;
    bool isRunning() override;
    bool forward(const SystemProtocol& message) override;
    ServiceType getType() const override;
    
    ~OrderEntryGateway() override = default;
};