#include "../../include/services/retransmission_service.h"
#include "../../include/services/order_entry_gateway.h"
#include <thread>
#include <chrono>

RetransmissionService* RetransmissionService::instance = nullptr;

RetransmissionService::RetransmissionService() : running(false){

}

RetransmissionService& RetransmissionService::getInstance() {
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lock(instanceMutex);
        if(instance == nullptr) {
            instance = new RetransmissionService();
        }
    }
    return *instance;
}

bool RetransmissionService::start() {
    running = true;
    instanceThread = std::thread(&RetransmissionService::run, this);
    return true;
}

bool RetransmissionService::stop() {
    running = false;
    if(instanceThread.joinable()) instanceThread.join();
    return true;
}

bool RetransmissionService::isRunning() {
    return running;
}

ServiceType RetransmissionService::getType() const {
    return ServiceType::RetransmissionService;
}

void RetransmissionService::processBatch(BatchSystemProtocol batch) {
    for (const auto& order : batch.get_all_messages())
        orders.push(order);
}


void RetransmissionService::run() {
    while(isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // add some logic later
    }
}

bool RetransmissionService::addConsumer(OrderBook* orderBook) {
    if (orderBook) {
        std::lock_guard<std::mutex> lock(instanceMutex);
        consumers.insert(orderBook);
    }
    return true;
}

bool RetransmissionService::addPoolConsumers(OrderBookPool& orderPool) {
    for (size_t i = 0; i < orderPool.getSize(); i++) {
        if (OrderBook* book = orderPool.get(i)) {
            addConsumer(book);
        }
    }
    return true;
}

std::vector<SystemProtocol> RetransmissionService::batchRequest(const int& fromId, const int& toId) {
    std::lock_guard<std::mutex> lock(instanceMutex);
    if (fromId > toId) return {};
    while (!orders.empty() && orders.front().transaction_id < fromId) orders.pop();
    std::vector<SystemProtocol> lostOrders = {};
    while(!orders.empty() && orders.front().transaction_id < toId) {
        lostOrders.push_back(orders.front());
        orders.pop();
    }
    return lostOrders;
}
