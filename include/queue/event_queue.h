#pragma once
#include <queue>
#include <vector>
#include <memory>
#include "../services/service_interface.h"
#include "../common/types.h"

class EventQueue {
private:
    std::queue<Event> queue;
    std::vector<std::unique_ptr<ServiceInterface>> consumers;
public:
    EventQueue() = default;
    // unique pointer here allows storing instances of an abstract class
    // may be useful for later
    void addConsumer(std::unique_ptr<ServiceInterface> consumer);
    bool push(const Event& event);
    Event poll();
};