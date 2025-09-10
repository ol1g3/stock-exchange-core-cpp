#include <iostream>
#include <memory>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "../include/client/client.h"
#include "../include/core/order_book.h"
#include "../include/queue/event_queue.h"
#include "../include/services/order_entry_gateway.h"
#include "../include/services/trade_notification_service.h"
#include "../include/services/retransmission_service.h"

int main(int argc, char const *argv[]) {
    srand(time(nullptr));

    std::cout << "Initializing exchange..." << std::endl;

    EventQueue eventQueue;

    auto notificationService = std::make_unique<TradeNotificationService>();
    notificationService->setEventQueue(&eventQueue);
    notificationService->start();

    OrderBookPool orderBookPool(1, eventQueue);
    
    RetransmissionService& retransmissionService = RetransmissionService::getInstance();
    retransmissionService.addPoolConsumers(orderBookPool);
    retransmissionService.start();
    
    OrderEntryGateway orderEntryGateway(orderBookPool); 
    orderEntryGateway.start();
    
    Client client(orderEntryGateway);
    notificationService->addClient(client);
    
    TradeNotificationService* notificationPtr = notificationService.get();
    eventQueue.addConsumer(std::move(notificationService));
    
    // Create orders
    client.createOrder();
    client.createOrder();
    client.createOrder();
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    retransmissionService.stop();
    orderEntryGateway.stop();
    
    std::cout << "Exchange shutting down." << std::endl;

    orderEntryGateway.stop();
    retransmissionService.stop();
    
    if (notificationPtr) {
        notificationPtr->stop();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}