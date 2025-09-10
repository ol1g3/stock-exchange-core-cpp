#include "performance_tests.h"
#include "../../include/core/fifo_strategy.h"
#include "../../include/core/pro_rata_strategy.h"
#include <algorithm>
#include <numeric>
#include <iostream>

PerformanceTests::PerformanceTests() 
    : rng(std::random_device{}()),
      userIdDist(1, 1000),
      priceDist(9500, 10500),
      quantityDist(1, 100),
      sideDist(0, 1) {}

ClientProtocol PerformanceTests::generateRandomOrder() {
    return ClientProtocol{
        userIdDist(rng),
        static_cast<bool>(sideDist(rng)),
        0,
        priceDist(rng),
        quantityDist(rng),
        static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count())
    };
}

PerformanceMetrics PerformanceTests::calculateMetrics(
    const std::vector<std::chrono::high_resolution_clock::time_point>& startTimes,
    const std::vector<std::chrono::high_resolution_clock::time_point>& endTimes,
    std::chrono::duration<double> totalDuration) {
    
    std::vector<double> latencies;
    latencies.reserve(std::min(startTimes.size(), endTimes.size()));
    
    for (size_t i = 0; i < std::min(startTimes.size(), endTimes.size()); ++i) {
        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(
            endTimes[i] - startTimes[i]).count();
        latencies.push_back(static_cast<double>(latency));
    }
    
    if (latencies.empty()) {
        return PerformanceMetrics{
            0.0, 0.0, 0.0, 0.0, 0, 0, totalDuration.count()
        };
    }
    
    std::sort(latencies.begin(), latencies.end());
    
    double avgLatency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
    double p95Latency = latencies[static_cast<size_t>(latencies.size() * 0.95)];
    double p99Latency = latencies[static_cast<size_t>(latencies.size() * 0.99)];
    
    return PerformanceMetrics{
        static_cast<double>(latencies.size()) / totalDuration.count(),
        avgLatency,
        p95Latency,
        p99Latency,
        latencies.size(),
        0,
        totalDuration.count()
    };
}

PerformanceMetrics PerformanceTests::testSingleThreadThroughput() {
    std::cout << "Starting OrderEntryGateway test..." << std::endl;
    
    EventQueue eventQueue;
    OrderBookPool orderBookPool(1, eventQueue);
    OrderEntryGateway gateway(orderBookPool);
    
    if (!gateway.start()) {
        std::cout << "Failed to start gateway" << std::endl;
        return PerformanceMetrics{0.0, 0.0, 0.0, 0.0, 0, 0, 0.0};
    }
    
    const size_t numOrders = 10000;
    std::vector<std::chrono::high_resolution_clock::time_point> startTimes, endTimes;
    startTimes.reserve(numOrders);
    endTimes.reserve(numOrders);
    
    auto testStart = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < numOrders; ++i) {
        auto orderStart = std::chrono::high_resolution_clock::now();
        ClientProtocol order = generateRandomOrder();
        
        startTimes.push_back(orderStart);
        gateway.acceptOrder(order);
        endTimes.push_back(std::chrono::high_resolution_clock::now());
    }
    
    auto testEnd = std::chrono::high_resolution_clock::now();
    
    std::cout << "Stopping gateway..." << std::endl;
    
    if (!gateway.stop()) {
        std::cout << "Warning: Failed to stop gateway properly" << std::endl;
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Gateway stopped, calculating metrics..." << std::endl;
    
    return calculateMetrics(startTimes, endTimes, testEnd - testStart);
}

PerformanceMetrics PerformanceTests::testMultiThreadProducerThroughput() {
    std::cout << "Starting multi-thread test..." << std::endl;
    
    EventQueue eventQueue;
    OrderBookPool orderBookPool(1, eventQueue);
    OrderEntryGateway gateway(orderBookPool);
    
    if (!gateway.start()) {
        return PerformanceMetrics{0.0, 0.0, 0.0, 0.0, 0, 0, 0.0};
    }
    
    const size_t numThreads = 4;
    const size_t ordersPerThread = 2500;
    std::vector<std::thread> producers;
    std::atomic<size_t> completedOrders{0};
    
    auto testStart = std::chrono::high_resolution_clock::now();
    
    for (size_t t = 0; t < numThreads; ++t) {
        producers.emplace_back([&, t]() {
            for (size_t i = 0; i < ordersPerThread; ++i) {
                ClientProtocol order = generateRandomOrder();
                order.userId = static_cast<uint32_t>(t * 1000 + i);
                gateway.acceptOrder(order);
                completedOrders.fetch_add(1);
            }
        });
    }
    
    for (auto& producer : producers) {
        producer.join();
    }
    
    auto testEnd = std::chrono::high_resolution_clock::now();
    
    std::cout << "Stopping multi-thread gateway..." << std::endl;
    gateway.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto duration = testEnd - testStart;
    return PerformanceMetrics{
        static_cast<double>(completedOrders.load()) / std::chrono::duration<double>(duration).count(),
        0.0, 0.0, 0.0,
        completedOrders.load(),
        0,
        std::chrono::duration<double>(duration).count()
    };
}

PerformanceMetrics PerformanceTests::testFifoStrategyPerformance() {
    std::cout << "Starting FIFO strategy test..." << std::endl;
    
    EventQueue eventQueue;
    auto strategy = std::make_unique<FIFOStrategy>();
    OrderBook orderBook(eventQueue, std::move(strategy));
    
    const size_t numOrders = 5000;
    std::vector<std::chrono::high_resolution_clock::time_point> startTimes, endTimes;
    startTimes.reserve(numOrders);
    endTimes.reserve(numOrders);
    
    auto testStart = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < numOrders; ++i) {
        ClientProtocol clientOrder = generateRandomOrder();
        SystemProtocol order = translate(clientOrder, static_cast<int>(i + 1));
        
        BatchSystemProtocol batch;
        batch.add_message(order);
        
        auto processStart = std::chrono::high_resolution_clock::now();
        startTimes.push_back(processStart);
        
        orderBook.process(batch);
        
        endTimes.push_back(std::chrono::high_resolution_clock::now());
    }
    
    auto testEnd = std::chrono::high_resolution_clock::now();
    
    std::cout << "FIFO test completed" << std::endl;
    
    return calculateMetrics(startTimes, endTimes, testEnd - testStart);
}

PerformanceMetrics PerformanceTests::testProRataStrategyPerformance() {
    std::cout << "Starting Pro-Rata strategy test..." << std::endl;
    
    EventQueue eventQueue;
    auto strategy = std::make_unique<ProRataStrategy>();
    OrderBook orderBook(eventQueue, std::move(strategy));
    
    const size_t numOrders = 5000;
    std::vector<std::chrono::high_resolution_clock::time_point> startTimes, endTimes;
    startTimes.reserve(numOrders);
    endTimes.reserve(numOrders);
    
    auto testStart = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < numOrders; ++i) {
        ClientProtocol clientOrder = generateRandomOrder();
        SystemProtocol order = translate(clientOrder, static_cast<int>(i + 1));
        
        BatchSystemProtocol batch;
        batch.add_message(order);
        
        auto processStart = std::chrono::high_resolution_clock::now();
        startTimes.push_back(processStart);
        
        orderBook.process(batch);
        
        endTimes.push_back(std::chrono::high_resolution_clock::now());
    }
    
    auto testEnd = std::chrono::high_resolution_clock::now();
    
    std::cout << "Pro-Rata test completed" << std::endl;
    
    return calculateMetrics(startTimes, endTimes, testEnd - testStart);
}

PerformanceMetrics PerformanceTests::testBatchProcessingEfficiency() {
    std::cout << "Starting batch processing test..." << std::endl;
    
    EventQueue eventQueue;
    OrderBookPool orderBookPool(1, eventQueue);
    OrderEntryGateway gateway(orderBookPool);
    
    if (!gateway.start()) {
        return PerformanceMetrics{0.0, 0.0, 0.0, 0.0, 0, 0, 0.0};
    }
    
    const size_t numBatches = 1000;
    const size_t ordersPerBatch = 10;
    
    auto testStart = std::chrono::high_resolution_clock::now();
    
    for (size_t batch = 0; batch < numBatches; ++batch) {
        for (size_t order = 0; order < ordersPerBatch; ++order) {
            ClientProtocol clientOrder = generateRandomOrder();
            gateway.acceptOrder(clientOrder);
        }
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    
    auto testEnd = std::chrono::high_resolution_clock::now();
    
    std::cout << "Stopping batch test gateway..." << std::endl;
    gateway.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto duration = testEnd - testStart;
    return PerformanceMetrics{
        static_cast<double>(numBatches * ordersPerBatch) / std::chrono::duration<double>(duration).count(),
        0.0, 0.0, 0.0,
        numBatches * ordersPerBatch,
        0,
        std::chrono::duration<double>(duration).count()
    };
}