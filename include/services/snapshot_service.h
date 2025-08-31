#pragma once
#include "service_interface.h"
#include "../core/order_book.h"
#include <queue>

class SnapshotService : public ServiceInterface {
private:
    std::thread snapshotThread;
    static std::mutex snapshotMutex;
    std::atomic<bool> running{false};
    std::queue<BatchSystemProtocol> batchSnapshots;
    std::chrono::milliseconds retentionPeriod;
    static SnapshotService *instance;

public:
    void run();
    SnapshotService() :
        retentionPeriod(std::chrono::milliseconds(10000)) {};
    SnapshotService(int retentionPeriodMs) :
        retentionPeriod(retentionPeriodMs) {};

    static SnapshotService& getInstance();


    bool start() override;
    bool stop() override;
    bool isRunning() override;
    ServiceType getType() const override;

    void process(BatchSystemProtocol batch);
    std::vector<BatchSystemProtocol> getSnaphots(uint64_t seqNumberFrom, uint64_t seqNumberTo);
    
    ~SnapshotService() override = default;
};