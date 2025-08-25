#include "../../include/services/service_interface.h"
#include "../../include/services/order_entry_gateway.h"
#include "../../include/core/order_book.h"

OrderEntryGateway* OrderEntryGateway::instance = nullptr;
OrderEntryGateway::OrderEntryGateway() : running(false), orderBookPool() {}
OrderEntryGateway::OrderEntryGateway(OrderBookPool orderBookPool) : running(false), orderBookPool(orderBookPool) {}

ServiceType OrderEntryGateway::initialize() {
    if (instance == nullptr) instance = new OrderEntryGateway(orderBookPool);
    return ServiceType::OrderEntryGateway;
}

bool OrderEntryGateway::start() {
    running = true;
    return true;
}

bool OrderEntryGateway::stop() {
    running = false;
    return true;
}

bool OrderEntryGateway::isRunning() {
    return running;
}

bool OrderEntryGateway::forward(const SystemProtocol& message) {
    OrderBook* dest = orderBookPool.get(0);
    if (dest == NULL) return false;
    return (*dest).process(message);
}

ServiceType OrderEntryGateway::getType() const {
    return ServiceType::OrderEntryGateway;
}

OrderEntryGateway* OrderEntryGateway::getInstance() {
    if (instance == nullptr) {
        instance = new OrderEntryGateway();
    }
    return instance;
}
