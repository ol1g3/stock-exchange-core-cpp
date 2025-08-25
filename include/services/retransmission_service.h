#include "service_interface.h"
#include "../core/order_book.h"

class RetransmissionService : public ServiceInterface {
private:
    static RetransmissionService instance;
    std::vector<OrderBook> consumers;
public:
    RetransmissionService();
    void addConsumer(const OrderBook& orderBook);
    const SystemProtocol& request(const int& transactionId);
};