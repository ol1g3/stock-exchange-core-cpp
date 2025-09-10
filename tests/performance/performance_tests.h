#pragma once
#include "../../include/core/order_book.h"
#include "../../include/services/order_entry_gateway.h"
#include "../../include/client/client.h"
#include <chrono>
#include <vector>
#include <thread>
#include <atomic>
#include <random>

struct PerformanceMetrics {
    double throughputOrdersPerSecond;
    double latencyAvgMicroseconds;
    double latencyP95Microseconds;
    double latencyP99Microseconds;
    size_t totalOrdersProcessed;
    size_t totalMatchesExecuted;
    double durationSeconds;
};

class PerformanceTests {
private:
    std::mt19937 rng;
    std::uniform_int_distribution<uint32_t> userIdDist;
    std::uniform_int_distribution<uint64_t> priceDist;
    std::uniform_int_distribution<int16_t> quantityDist;
    std::uniform_int_distribution<int> sideDist;

    ClientProtocol generateRandomOrder();
    PerformanceMetrics calculateMetrics(
        const std::vector<std::chrono::high_resolution_clock::time_point>& startTimes,
        const std::vector<std::chrono::high_resolution_clock::time_point>& endTimes,
        std::chrono::duration<double> totalDuration
    );

public:
    PerformanceTests();
    
    PerformanceMetrics testSingleThreadThroughput();
    PerformanceMetrics testMultiThreadProducerThroughput();
    PerformanceMetrics testFifoStrategyPerformance();
    PerformanceMetrics testProRataStrategyPerformance();
    PerformanceMetrics testBatchProcessingEfficiency();
};