#include <vector>
#include "service_interface.h"
#include "../client/client.h"
#include "../common/types.h"
#include "../queue/event_queue.h"
#include <thread>
#include <atomic>

class TradeNotificationService : public ServiceInterface {
private:
    std::vector<Client> clients;
    std::thread processingThread;
    std::atomic<bool> running{false};
    EventQueue* queue;
public:
    TradeNotificationService() : queue(nullptr) {}
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