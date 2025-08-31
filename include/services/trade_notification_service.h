#pragma once
#include "service_interface.h"
#include "../common/types.h"
#include "../client/client.h"
#include <vector>
#include <thread>
#include <atomic>

class EventQueue;
struct SystemProtocol;

/**
 * @brief Service for notifying clients about trade events.
 */
class TradeNotificationService : public ServiceInterface {
private:
    std::vector<Client> clients;
    std::thread processingThread;
    std::atomic<bool> running{false};
    EventQueue* queue;
public:
    TradeNotificationService();

    void addClient(Client c);
    void notify(Client c, Event e);
    void setEventQueue(EventQueue* eventQueue);

    ServiceType initialize();
    bool start() override;
    bool stop() override;
    bool isRunning() override;
    bool run();
    bool forward(const SystemProtocol& message);
    ServiceType getType() const override;

    ~TradeNotificationService() override = default;
};