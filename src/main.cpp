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

int main(int argc, char const *argv[]){
    srand(time(nullptr));

    std::cout << "Initializing exchange..." << std::endl;

    EventQueue eventQueue;
    eventQueue.addConsumer(std::make_unique<TradeNotificationService>());
    OrderBookPool orderBookPool(1, eventQueue); 

    OrderEntryGateway orderEntryGateway(orderBookPool); 

    Client client(orderEntryGateway); 
    
    if (client.createOrder()) {
        std::cout << "Order accepted by gateway." << std::endl;
    }
    else {
        std::cout << "Order rejected by gateway." << std::endl;
    }

    std::cout << "Exchange shutting down." << std::endl;
    return 0;
}