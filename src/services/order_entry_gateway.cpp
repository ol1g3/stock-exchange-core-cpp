#include "../../include/services/order_entry_gateway.h"
#include "../../include/common/message.h"

OrderEntryGateway::OrderEntryGateway(OrderBookPool& pool) 
    : running(false), orderBookPool(pool) {}

ServiceType OrderEntryGateway::initialize() {
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

bool OrderEntryGateway::acceptOrder(const ClientProtocol& message){
    SystemProtocol systemMessage = translate(message);
    return this->forward(systemMessage);
}

bool OrderEntryGateway::forward(const SystemProtocol& message) {
    OrderBook* dest = orderBookPool.get(0);
    if (dest == nullptr) return false;
    return dest->process(message);
}

ServiceType OrderEntryGateway::getType() const {
    return ServiceType::OrderEntryGateway;
}