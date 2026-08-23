#pragma once
#include "../services/order_entry_gateway.h"

class Client {
private:
    static int next_id;
protected:
    int id;
    OrderEntryGateway& orderEntryGateway;
    bool submit(bool side, uint64_t price, int16_t quantity, TimeInForce timeInForce);
public:
    Client(OrderEntryGateway& orderEntryGateway);
    int getId() const;
    virtual bool createOrder() = 0;
    virtual void getNotification(const Event& event) = 0;
    virtual void receiveQuoteUpdate(QuoteSummary summary) = 0;
    bool operator==(const Client& b) const;
    virtual ~Client() = default;
};
