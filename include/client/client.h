#pragma once
#include "../services/order_entry_gateway.h"

class Client {
private:
    static int next_id;
    int id;
    OrderEntryGateway& orderEntryGateway;
public:
    Client(OrderEntryGateway& orderEntryGateway);
    bool createOrder();
    void getNotification(const Event& event);
    void receiveQuoteUpdate(QuoteSummary summary);
    bool operator==(const Client& b) const;
    ~Client() = default;
};