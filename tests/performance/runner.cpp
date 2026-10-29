#include "performance_tests.h"
#include <iostream>
#include <iomanip>

void printMetrics(const std::string& testName, const PerformanceMetrics& metrics) {
    std::cout << "\n=== " << testName << " ===" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Throughput: " << metrics.throughputOrdersPerSecond << " orders/sec" << std::endl;
    std::cout << "Total Orders: " << metrics.totalOrdersProcessed << " orders" << std::endl;
    std::cout << "Duration: " << metrics.durationSeconds << " seconds" << std::endl;
    
    if (metrics.latencyAvgMicroseconds > 0) {
        std::cout << "Avg Latency: " << metrics.latencyAvgMicroseconds << " μs" << std::endl;
        std::cout << "P95 Latency: " << metrics.latencyP95Microseconds << " μs" << std::endl;
        std::cout << "P99 Latency: " << metrics.latencyP99Microseconds << " μs" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    PerformanceTests tests;

    // --roundtrip [batchIntervalMs] [samples]: one config per process, because the gateway's
    // sequence counter is process-wide and a fresh order book expects it to start at 1.
    if (argc > 1 && std::string(argv[1]) == "--roundtrip") {
        long intervalMs = argc > 2 ? std::stol(argv[2]) : 2000;
        size_t samples = argc > 3 ? static_cast<size_t>(std::stol(argv[3])) : 10;
        auto metrics = tests.testRoundTripLatency(std::chrono::milliseconds(intervalMs), samples);
        printMetrics("Round Trip (submit -> client notification)", metrics);
        return 0;
    }
    bool perfOnly = (argc > 1 && std::string(argv[1]) == "--perf-only");
    
    std::cout << "Stock Exchange Performance Tests" << std::endl;
    std::cout << "================================" << std::endl;
    
    if (perfOnly) {
        std::cout << "\nRunning core performance tests only..." << std::endl;
        
        auto metrics1 = tests.testSingleThreadThroughput();
        printMetrics("Single Thread Throughput", metrics1);
        
        auto metrics2 = tests.testMultiThreadProducerThroughput();
        printMetrics("Multi-Thread Producer", metrics2);
        
        auto metrics3 = tests.testBatchProcessingEfficiency();
        printMetrics("Batch Processing", metrics3);
    } else {
        std::cout << "\nRunning full benchmark suite..." << std::endl;
        
        auto metrics1 = tests.testSingleThreadThroughput();
        printMetrics("Single Thread Throughput", metrics1);
        
        auto metrics2 = tests.testMultiThreadProducerThroughput();
        printMetrics("Multi-Thread Producer", metrics2);
        
        auto metrics3 = tests.testFifoStrategyPerformance();
        printMetrics("FIFO Strategy", metrics3);
        
        auto metrics4 = tests.testProRataStrategyPerformance();
        printMetrics("Pro-Rata Strategy", metrics4);
        
        auto metrics5 = tests.testBatchProcessingEfficiency();
        printMetrics("Batch Processing", metrics5);
    }
    
    std::cout << "\nBenchmark complete!" << std::endl;
    return 0;
}