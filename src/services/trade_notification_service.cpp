#include <vector>
#include "../../include/services/trade_notification_service.h"
#include <algorithm> // Required for std::find

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
    return true;
}

bool TradeNotificationService::stop() {
    return true;
}

bool TradeNotificationService::isRunning() {
    return true;
}

bool TradeNotificationService::forward(const SystemProtocol& message) {
    (void)message; // Suppress unused parameter warning
    return true;
}