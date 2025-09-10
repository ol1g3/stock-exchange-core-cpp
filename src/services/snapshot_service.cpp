#include "../../include/services/snapshot_service.h"

#include <set>
#include <vector>

const int maxSnapshotNumber = 1000;
std::mutex SnapshotService::snapshotMutex;

SnapshotService::SnapshotService() : running(false) {}

bool SnapshotService::start() {
    running = true;
    snapshotThread = std::thread(&SnapshotService::run, this);
    return true;
}

bool SnapshotService::stop() {
    running = false;
    if(snapshotThread.joinable()) snapshotThread.join();
    return true;
}

ServiceType SnapshotService::getType() const {
    return ServiceType::SnapshotService;
}

bool SnapshotService::isRunning() {
    return running;
}

void SnapshotService::process(BatchSystemProtocol batch) {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    batchSnapshots.push(batch);
    if (static_cast<int>(batchSnapshots.size()) > maxSnapshotNumber) {
        batchSnapshots.pop();
    }
}

void SnapshotService::run() {
    while (isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // add some logic later
    }
}

SnapshotService& SnapshotService::getInstance() {
    static SnapshotService instance;
    return instance;
}

std::vector<BatchSystemProtocol> SnapshotService::getSnapshots(uint64_t seqNumberFrom, uint64_t seqNumberTo) {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    std::vector<BatchSystemProtocol> result;

    while (!batchSnapshots.empty()) {
        const BatchSystemProtocol& batch = batchSnapshots.front();
        uint64_t batchSeqNumber = batch.get_message(0).seq_number;
        if (batchSeqNumber < seqNumberFrom) continue;
        if (batchSeqNumber > seqNumberTo) break;
        result.push_back(batch);
        batchSnapshots.pop();
    }
    return result;
}