#include <iostream>
#include "../../include/queue/event_queue.h"
#include "../../include/core/order_book.h"
#include "../../include/client/client.h"

static std::mutex queueMutex;

bool EventQueue::push(const Event& event) {
    std::lock_guard<std::mutex> lock(queueMutex);
    queue.push(event);
    return true;
}

void EventQueue::addConsumer(std::unique_ptr<ServiceInterface> consumer) {
    consumers.push_back(std::move(consumer));
}

std::optional<Event> EventQueue::poll() {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (queue.empty()) {
        return std::nullopt;
    }

    Event event = queue.front();
    queue.pop();
    return event;
}