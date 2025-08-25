#include <iostream>
#include "../../include/queue/event_queue.h"
#include "../../include/core/order_book.h"
#include "../../include/client/client.h"

bool EventQueue::push(const Event& event) {
    queue.push(event);
    std::cout << event.timestamp << "\n"; 
    return true;
}

void EventQueue::addConsumer(std::unique_ptr<ServiceInterface> consumer) {
    consumers.push_back(std::move(consumer));
}

Event EventQueue::poll() {
    if (queue.empty()) {
        return Event();
    }
    Event event = queue.front();
    queue.pop();
    return event;
}