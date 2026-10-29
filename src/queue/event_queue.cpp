#include <iostream>
#include <condition_variable>
#include <mutex>
#include "../../include/queue/event_queue.h"
#include "../../include/core/order_book.h"
#include "../../include/client/client.h"

static std::mutex queueMutex;
static std::condition_variable queueCv;

bool EventQueue::push(const Event& event) {
    std::lock_guard<std::mutex> lock(queueMutex);
    queue.push(event);
    queueCv.notify_one();
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

std::optional<Event> EventQueue::pollWait(std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(queueMutex);
    queueCv.wait_for(lock, timeout, [this] { return !queue.empty(); });
    if (queue.empty()) {
        return std::nullopt;
    }

    Event event = queue.front();
    queue.pop();
    return event;
}

void EventQueue::wake() {
    queueCv.notify_all();
}
