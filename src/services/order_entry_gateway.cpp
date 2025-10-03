#include "../../include/services/order_entry_gateway.h"
#include "../../include/common/message.h"
#include <iostream>
#include <thread>
#include <chrono>

OrderEntryGateway::OrderEntryGateway(OrderBookPool& pool) 
    : running(false), orderBookPool(pool) {}

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
    
    running = false;
    
    {
        std::lock_guard<std::mutex> lock(batchMutex);
        if(batch.batch[0] > 0) {
            std::cout << "Processing final batch of " << (int)batch.batch[0] << " orders before shutdown" << std::endl;
            orderBookPool.process(batch);
        }
    }
    
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
    SystemProtocol systemOrder = translate(order, transactionCounter++);
    
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
    return true;
}

bool OrderEntryGateway::run() {
    while(running) {
        std::this_thread::sleep_for(batchInterval);
        if(!running) break;
        std::lock_guard<std::mutex> lock(batchMutex);
        
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