#include "../../include/client/client.h"
#include <chrono>
#include <cstdlib>
#include <iostream>

int Client::next_id = 1;
Client::Client(OrderEntryGateway& gateway) 
    : id(next_id ++), orderEntryGateway(gateway) {}

bool Client::operator==(const Client& b) const {
    return this->id == b.id;
}

bool Client::createOrder(){
    bool side = rand() % 2 == 1;
    uint64_t price = 98 + (rand() % 5);
    int16_t quantity = 1 + (rand() % 100);
    uint64_t timestamp = std::chrono::system_clock::now().time_since_epoch().count();

    ClientProtocol order(
        this->id,
        side,
        false, // padding
        price,
        quantity,
        timestamp
    );

    std::cout << "Client " << this->id << " sending " << (side ? "BUY" : "SELL") 
              << " order: " << quantity << " at price " << price << std::endl;

    return orderEntryGateway.acceptOrder(order);
}