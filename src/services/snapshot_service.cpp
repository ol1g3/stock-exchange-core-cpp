#include "../../include/services/snapshot_service.h"
#include <vector>

bool SnapshotService::forward(const SystemProtocol& message) {
    while(orderQueue.size() && orderQueue.front().timestamp + retentionPeriod.count() < message.timestamp) orderQueue.pop();

    orderQueue.push(message);
}

std::vector<SystemProtocol> SnapshotService::getSnapshot() {
    std::vector<SystemProtocol> result;
    result.reserve(orderQueue.size());
    
    std::queue<SystemProtocol> tempQueue = orderQueue;

    while (!tempQueue.empty()) {
        result.push_back(tempQueue.front());
        tempQueue.pop();
    }
    return result;
}