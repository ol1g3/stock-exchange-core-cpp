#include "../../include/client/market_maker.h"
#include <limits>

MarketMaker::MarketMaker(OrderEntryGateway& gateway,
                         uint64_t fairValue,
                         uint64_t halfSpread,
                         int16_t quoteSize,
                         int32_t maxInventory)
    : Client(gateway),
      fairValue(fairValue),
      halfSpread(halfSpread),
      quoteSize(quoteSize),
      maxInventory(maxInventory) {}

uint64_t MarketMaker::quoteCentre() const {
    int32_t position = inventory.load();
    int64_t skew = (static_cast<int64_t>(halfSpread) * position) / (maxInventory > 0 ? maxInventory : 1);
    int64_t centre = static_cast<int64_t>(fairValue) - skew;
    if (centre <= static_cast<int64_t>(halfSpread)) {
        centre = static_cast<int64_t>(halfSpread) + 1;
    }
    return static_cast<uint64_t>(centre);
}

bool MarketMaker::createOrder() {
    uint64_t centre = quoteCentre();
    uint64_t bid = centre - halfSpread;
    uint64_t ask = centre + halfSpread;

    if (!needsRequote.exchange(false) && bid == lastBid && ask == lastAsk) {
        return true;
    }
    lastBid = bid;
    lastAsk = ask;

    int32_t position = inventory.load();
    bool ok = true;
    if (position < maxInventory) {
        ok = submit(true, bid, quoteSize, TimeInForce::GTC) && ok;
    }
    if (position > -maxInventory) {
        ok = submit(false, ask, quoteSize, TimeInForce::GTC) && ok;
    }
    return ok;
}

void MarketMaker::getNotification(const Event& event) {
    if (event.status != OrderStatus::Filled && event.status != OrderStatus::PartiallyFilled) {
        return;
    }

    int64_t quantity = event.quantity;
    int64_t notional = static_cast<int64_t>(event.price) * quantity;

    if (event.side) {
        inventory.fetch_add(static_cast<int32_t>(quantity));
        cash.fetch_sub(notional);
    }
    else {
        inventory.fetch_sub(static_cast<int32_t>(quantity));
        cash.fetch_add(notional);
    }
    fills.fetch_add(1);
    sharesTraded.fetch_add(quantity);
    needsRequote.store(true);
}

void MarketMaker::receiveQuoteUpdate(QuoteSummary summary) {
    if (summary.highestBid == 0 || summary.lowestAsk == std::numeric_limits<uint64_t>::max()) {
        return;
    }
    fairValue = (summary.highestBid + summary.lowestAsk) / 2;
}

int32_t MarketMaker::getInventory() const {
    return inventory.load();
}

int64_t MarketMaker::getCash() const {
    return cash.load();
}

int64_t MarketMaker::getMarkToMarketPnl() const {
    return cash.load() + static_cast<int64_t>(inventory.load()) * static_cast<int64_t>(fairValue);
}

uint64_t MarketMaker::getFills() const {
    return fills.load();
}

int64_t MarketMaker::getSharesTraded() const {
    return sharesTraded.load();
}
