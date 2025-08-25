#pragma once
#include "service_interface.h"
#include "../core/order_book.h"

class OrderEntryGateway : public ServiceInterface {
private:
    static OrderEntryGateway* instance;
    bool running;
    OrderBookPool orderBookPool;
    
    OrderEntryGateway();
    OrderEntryGateway(OrderBookPool orderBookPool);
    
public:
    ServiceType initialize() override;
    bool start() override;
    bool stop() override;
    bool isRunning() override;
    bool forward(const SystemProtocol& message) override;
    ServiceType getType() const override;
    static OrderEntryGateway* getInstance();
    
    ~OrderEntryGateway() = default;
};