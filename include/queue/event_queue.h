#pragma once
#include <vector>
#include "../services/trade_notification_service.h"
#include "../common/types.h"

class EventQueue {
private:
    std::vector<Event> queue;
    std::vector<TradeNotificationService> consumers;
public:
    bool push(const Event& event);
    Event poll();
};