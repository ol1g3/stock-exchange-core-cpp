#include "../../include/client/noise_trader.h"

NoiseTrader::NoiseTrader(OrderEntryGateway& gateway,
                         uint64_t referencePrice,
                         uint64_t aggression,
                         int16_t minQuantity,
                         int16_t maxQuantity,
                         uint32_t seed)
    : Client(gateway),
      referencePrice(referencePrice),
      aggression(aggression),
      minQuantity(minQuantity),
      maxQuantity(maxQuantity),
      rng(seed) {}

bool NoiseTrader::createOrder() {
    std::bernoulli_distribution sideDist(0.5);
    std::uniform_int_distribution<int> quantityDist(minQuantity, maxQuantity);

    bool side = sideDist(rng);
    uint64_t price = side ? referencePrice + aggression : referencePrice - aggression;
    int16_t quantity = static_cast<int16_t>(quantityDist(rng));

    ordersSent.fetch_add(1);
    return submit(side, price, quantity, TimeInForce::IOC);
}

void NoiseTrader::getNotification(const Event& event) {
    if (event.status == OrderStatus::Cancelled) {
        cancelledShares.fetch_add(event.quantity);
        return;
    }
    if (event.status != OrderStatus::Filled && event.status != OrderStatus::PartiallyFilled) {
        return;
    }
    fills.fetch_add(1);
    sharesTraded.fetch_add(event.quantity);
    netPosition.fetch_add(event.side ? event.quantity : -event.quantity);
}

void NoiseTrader::receiveQuoteUpdate(QuoteSummary summary) {
    (void)summary;
}

uint64_t NoiseTrader::getOrdersSent() const {
    return ordersSent.load();
}

uint64_t NoiseTrader::getFills() const {
    return fills.load();
}

int64_t NoiseTrader::getSharesTraded() const {
    return sharesTraded.load();
}

int64_t NoiseTrader::getNetPosition() const {
    return netPosition.load();
}

int64_t NoiseTrader::getCancelledShares() const {
    return cancelledShares.load();
}
