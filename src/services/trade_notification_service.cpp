#include <vector>
#include "../../include/services/trade_notification_service.h"
#include <algorithm>
#include <thread>
#include <atomic>

TradeNotificationService::TradeNotificationService() : queue(nullptr) {}

void TradeNotificationService::addClient(Client c){
    clients.push_back(c);
}

void TradeNotificationService::notify(Client c, Event e){
    if(find(clients.begin(), clients.end(), c) == clients.end()) return;
    c.getNotification(e);
}

ServiceType TradeNotificationService::getType() const {
    return ServiceType::TradeNotificationService;
}

ServiceType TradeNotificationService::initialize() {
    return ServiceType::TradeNotificationService;
}

bool TradeNotificationService::start() {
    running = true;
    processingThread = std::thread(&TradeNotificationService::run, this);
    return true;
}

bool TradeNotificationService::stop() {
    running = false;
    if (processingThread.joinable()) {
        processingThread.join();
    }
    return true;
}

bool TradeNotificationService::isRunning() {
    return running;
}

bool TradeNotificationService::run() {
    while (running) {
        if (queue) {
            Event event = queue->poll();
            if (event.orderId > 0 && event.orderId != UINT32_MAX) {
                std::cout << "TradeNotificationService: Processing event " << event.orderId << std::endl;
                for (auto& client : clients) {
                    client.getNotification(event);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return true;
}

bool TradeNotificationService::forward(const SystemProtocol& message) {
    (void)message; // Suppress unused parameter warning
    return true;
}

void TradeNotificationService::setEventQueue(EventQueue* eventQueue) {
    queue = eventQueue;
}