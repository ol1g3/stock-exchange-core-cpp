#include <vector>
#include "../../include/services/trade_notification_service.h"
#include <algorithm>
#include <thread>
#include <atomic>
#include "../../include/queue/event_queue.h"

TradeNotificationService::TradeNotificationService() : queue(nullptr) {}

void TradeNotificationService::addClient(Client* c){
    if (c) clients.push_back(c);
}

void TradeNotificationService::notify(Client* c, const Event& e){
    if(find(clients.begin(), clients.end(), c) == clients.end()) return;
    c->getNotification(e);
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
    if (queue) queue->wake();
    if (processingThread.joinable()) {
        processingThread.join();
    }
    if (queue) {
        while (std::optional<Event> event = queue->poll()) {
            dispatch(*event);
        }
    }
    return true;
}

void TradeNotificationService::dispatch(const Event& event) {
    for (auto* client : clients) {
        if (static_cast<uint32_t>(client->getId()) == event.userId) {
            client->getNotification(event);
        }
    }
}

bool TradeNotificationService::isRunning() {
    return running;
}

bool TradeNotificationService::run() {
    while (running) {
        if (queue) {
            if (std::optional<Event> event = queue->pollWait(std::chrono::milliseconds(50))) {
                dispatch(*event);
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
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