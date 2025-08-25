#pragma once
#include "service_interface.h"
#include "../core/order_book.h"
#include <queue>

class SnapshotService : public ServiceInterface {
private:
    std::queue<SystemProtocol> orderQueue;
    std::chrono::milliseconds retentionPeriod;
public:
    SnapshotService(int retentionPeriod) : orderQueue(), retentionPeriod(retentionPeriod) {};
    bool forward(const SystemProtocol& message) override;
    
    void snap();
    std::vector<SystemProtocol> getSnapshot();
    
    ~SnapshotService() override = default;
};