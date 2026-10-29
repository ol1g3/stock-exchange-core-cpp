#include "../../include/services/order_entry_gateway.h"
#include "../../include/common/message.h"
#include <iostream>
#include <thread>
#include <chrono>

OrderEntryGateway::OrderEntryGateway(OrderBookPool& pool, std::chrono::milliseconds interval)
    : running(false), orderBookPool(pool), batchInterval(interval) {}

bool OrderEntryGateway::start() {
    if(running) {
        return true;
    }
    
    running = true;
    batchThread = std::thread(&OrderEntryGateway::run, this);
    
    return true;
}

bool OrderEntryGateway::stop() {
    if(!running) {
        return true;
    }
    
    {
        std::lock_guard<std::mutex> lock(batchMutex);
        running = false;
        if(batch.batch[0] > 0) {
            std::cout << "Processing final batch of " << (int)batch.batch[0] << " orders before shutdown" << std::endl;
            orderBookPool.process(batch);
        }
    }
    
    batchCv.notify_all();
    if(batchThread.joinable()) {
        batchThread.join();
    }
    return true;
}

bool OrderEntryGateway::isRunning() {
    return running;
}

bool OrderEntryGateway::acceptOrder(const ClientProtocol& order) {
    if(!running) {
        return false;
    }

    if (order.price <= 0 || order.quantity <= 0 || order.userId <= 0) {
        std::cerr << "Invalid order rejected: " 
          << (order.price <= 0 ? "Invalid price " : "")
          << (order.quantity <= 0 ? "Invalid quantity " : "") 
          << std::endl;
        return false;
    }

    static int transactionCounter = 1;
    ClientProtocol stamped = order;
    stamped.timestamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    SystemProtocol systemOrder = translate(stamped, transactionCounter++);
    
    std::lock_guard<std::mutex> lock(batchMutex);
    
    if(!batch.add_message(systemOrder)) {
        std::cout << "Batch full (" << (int)batch.batch[0] << " orders), processing" << std::endl;
        orderBookPool.process(batch);
        
        batch = BatchSystemProtocol();
        if(!batch.add_message(systemOrder)) {
            std::cout << "Failed to add order to new batch" << std::endl;
            return false;
        }
    }
    batchCv.notify_one();
    return true;
}

bool OrderEntryGateway::run() {
    while(running) {
        std::unique_lock<std::mutex> lock(batchMutex);
        // flush as soon as an order arrives; batchInterval only bounds how long we idle
        batchCv.wait_for(lock, batchInterval, [this] { return !running || batch.batch[0] > 0; });
        if(!running) break;
        
        if(batch.batch[0] > 0) {
            std::cout << "Processing batch of " << (int)batch.batch[0] << " orders (periodic)" << std::endl;
            orderBookPool.process(batch);
            batch = BatchSystemProtocol();
        }
    }
    return true;
}

ServiceType OrderEntryGateway::getType() const {
    return ServiceType::OrderEntryGateway;
}

ServiceType OrderEntryGateway::initialize() {
    return ServiceType::OrderEntryGateway;
}

bool OrderEntryGateway::forward(const SystemProtocol& message) {
    std::lock_guard<std::mutex> lock(batchMutex);
    
    if(!batch.add_message(message)) {
        orderBookPool.process(batch);
        batch = BatchSystemProtocol();
        return batch.add_message(message);
    }
    
    return true;
}

void OrderEntryGateway::process(const BatchSystemProtocol& incomingBatch) {
    orderBookPool.process(incomingBatch);
}