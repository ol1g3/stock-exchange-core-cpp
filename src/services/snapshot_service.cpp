#include "../../include/services/snapshot_service.h"
#include <vector>

SnapshotService* SnapshotService::instance = nullptr;

bool SnapshotService::start() {
    running = true;
    snapshotThread = std::thread(&SnapshotService::run, this);
    return true;
}

bool SnapshotService::stop() {
    running = false;
    return true;
}

ServiceType SnapshotService::getType() const {
    return ServiceType::SnapshotService;
}

bool SnapshotService::isRunning() {
    return running;
}

void SnapshotService::process(BatchSystemProtocol batch) {
    batchSnapshots.push(batch);
}

void SnapshotService::run() {
    while (isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // add some logic later
    }
}

SnapshotService& SnapshotService::getInstance() {
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lock(snapshotMutex);
        if(instance == nullptr) {
            instance = new SnapshotService();
        }
    }
    return *instance;
}


std::vector<BatchSystemProtocol> SnapshotService::getSnaphots(uint64_t seqNumberFrom, uint64_t seqNumberTo) {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    std::vector<BatchSystemProtocol> result;

    while (!batchSnapshots.empty()) {
        BatchSystemProtocol batch = batchSnapshots.front();
        uint64_t batchSeqNumber = batch.get_message(0).seq_number;
        batchSnapshots.pop();
        if (batchSeqNumber < seqNumberFrom) continue;
        if (batchSeqNumber > seqNumberTo) break;
        result.push_back(batch);
    }
    return result;
}
