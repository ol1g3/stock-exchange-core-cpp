#include <vector>
#include "service_interface.h"
#include "../client/client.h"
#include "../common/types.h"


class TradeNotificationService : public ServiceInterface {
private:
    std::vector<Client> clients;
public:
    TradeNotificationService() = default;
    void addClient(Client c);
    void notify(Client c, Event e);
    
    ServiceType initialize() override;
    bool start() override;
    bool stop() override;
    bool isRunning() override;
    bool forward(const SystemProtocol& message) override;
    ServiceType getType() const override;
    ~TradeNotificationService() override = default;
};