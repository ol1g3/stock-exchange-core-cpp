#pragma once
#include "client.h"
#include <atomic>

class MarketMaker : public Client {
private:
    uint64_t fairValue;
    uint64_t halfSpread;
    int16_t quoteSize;
    int32_t maxInventory;
    std::atomic<int32_t> inventory{0};
    std::atomic<int64_t> cash{0};
    std::atomic<uint64_t> fills{0};
    std::atomic<int64_t> sharesTraded{0};
    uint64_t lastBid{0};
    uint64_t lastAsk{0};
    std::atomic<bool> needsRequote{true};

    uint64_t quoteCentre() const;

public:
    MarketMaker(OrderEntryGateway& gateway,
                uint64_t fairValue,
                uint64_t halfSpread,
                int16_t quoteSize,
                int32_t maxInventory);

    bool createOrder() override;
    void getNotification(const Event& event) override;
    void receiveQuoteUpdate(QuoteSummary summary) override;

    int32_t getInventory() const;
    int64_t getCash() const;
    int64_t getMarkToMarketPnl() const;
    uint64_t getFills() const;
    int64_t getSharesTraded() const;

    ~MarketMaker() override = default;
};
