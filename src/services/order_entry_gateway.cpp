#include "../../include/services/order_entry_gateway.h"
#include "../../include/common/message.h"

OrderEntryGateway::OrderEntryGateway(OrderBookPool& pool) 
    : running(false), orderBookPool(pool) {}

ServiceType OrderEntryGateway::initialize() {
    return ServiceType::OrderEntryGateway;
}

bool OrderEntryGateway::start() {
    running = true;
    batchThread = std::thread(&OrderEntryGateway::run, this);
    return true;
}

bool OrderEntryGateway::stop() {
    running = false;
    if(batchThread.joinable()) batchThread.join();
    return true;
}

bool OrderEntryGateway::isRunning() {
    return running;
}

bool OrderEntryGateway::acceptOrder(const ClientProtocol& message){
    SystemProtocol systemMessage = translate(message);
    {
        std::lock_guard<std::mutex> lock (batchMutex);
        if (!batch.add_message(systemMessage)) return false;
    }
    return true;
}

bool OrderEntryGateway::run() {
    while(isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // change later
        BatchSystemProtocol batchToSend;
        {
            std::lock_guard<std::mutex> lock (batchMutex);
            std::swap(batchToSend, batch);
        }
        if (batchToSend.batch[0] > 0){
            OrderBook* dest = orderBookPool.get(0);
            if (dest == nullptr) return false;
            return dest->process(batchToSend);
        }
    }
    return true;
}

ServiceType OrderEntryGateway::getType() const {
    return ServiceType::OrderEntryGateway;
}
