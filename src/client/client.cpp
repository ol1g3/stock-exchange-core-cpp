#include "../../include/client/client.h"
#include <chrono>

int Client::next_id = 1;
Client::Client(OrderEntryGateway& gateway) 
    : id(next_id++), orderEntryGateway(gateway) {}

int Client::getId() const {
    return id;
}

bool Client::operator==(const Client& b) const {
    return this->id == b.id;
}

bool Client::submit(bool side, uint64_t price, int16_t quantity, TimeInForce timeInForce) {
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();

    ClientProtocol order(
        static_cast<uint32_t>(this->id),
        side,
        static_cast<uint8_t>(timeInForce),
        price,
        quantity,
        timestamp
    );

    return orderEntryGateway.acceptOrder(order);
}
