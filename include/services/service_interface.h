#pragma once
#include "../common/message.h"
#include <string>

enum class ServiceType {
    OrderEntryGateway,
    QuoteService,
    SnapshotService,
    TradeNotificationService,
    RetransmissionService
};

class ServiceInterface {
public:
    virtual ServiceType initialize() = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool isRunning() = 0;

    virtual bool forward(const SystemProtocol& message) = 0;

    virtual ServiceType getType() const = 0;
    virtual ~ServiceInterface() = default;
};