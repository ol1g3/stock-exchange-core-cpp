#include <iostream>
#include "../../include/queue/event_queue.h"
#include "../../include/core/order_book.h"

bool EventQueue::push(const Event& event) {
    queue.push_back(event);
    return true;
}

Event EventQueue::poll() {
    if (queue.empty()) {
        return Event();
    }
    Event event = queue.front();
    queue.erase(queue.begin());
    return event;
}