#pragma once
#include <queue>
#include "../services/trade_notification_service.h"
#include "../common/types.h"

class EventQueue {
private:
    std::queue<Event> queue;
    std::vector<TradeNotificationService> consumers;
public:
    bool push(const Event& event);
    Event poll();
};