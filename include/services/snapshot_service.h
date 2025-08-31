#pragma once
#include "service_interface.h"
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>

class BatchSystemProtocol;
class OrderBook;

/**
 * @brief Service for storing and retrieving order book snapshots for recovery.
 */
class SnapshotService : public ServiceInterface {
private:
    // Singleton pattern
    static SnapshotService* instance;
    static std::mutex snapshotMutex;

    std::thread snapshotThread;
    std::atomic<bool> running{false};
    std::queue<BatchSystemProtocol> batchSnapshots;
    std::chrono::milliseconds retentionPeriod;
    void run();

public:
    SnapshotService();
    explicit SnapshotService(int retentionPeriodMs);
    static SnapshotService& getInstance();


    bool start() override;
    bool stop() override;
    bool isRunning() override;
    ServiceType getType() const override;

    void process(BatchSystemProtocol batch);
    std::vector<BatchSystemProtocol> getSnapshots(uint64_t seqNumberFrom, uint64_t seqNumberTo);

    ~SnapshotService() override = default;
};