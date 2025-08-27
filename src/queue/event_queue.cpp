#include <iostream>
#include "../../include/queue/event_queue.h"
#include "../../include/core/order_book.h"
#include "../../include/client/client.h"

static std::mutex queueMutex;

bool EventQueue::push(const Event& event) {
    std::lock_guard<std::mutex> lock(queueMutex);
    queue.push(event);
    std::cout << "EventQueue: Pushed event " << event.orderId << std::endl;
    return true;
}

void EventQueue::addConsumer(std::unique_ptr<ServiceInterface> consumer) {
    consumers.push_back(std::move(consumer));
}

Event EventQueue::poll() {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (queue.empty()) {
        return Event{};
    }
    Event event = queue.front();
    queue.pop();
    std::cout << "EventQueue: Polled event " << event.orderId << std::endl;
    return event;
}