#pragma once
#include "client.h"
#include <atomic>
#include <random>

class NoiseTrader : public Client {
private:
    uint64_t referencePrice;
    uint64_t aggression;
    int16_t minQuantity;
    int16_t maxQuantity;
    std::mt19937 rng;
    std::atomic<uint64_t> ordersSent{0};
    std::atomic<uint64_t> fills{0};
    std::atomic<int64_t> sharesTraded{0};
    std::atomic<int64_t> netPosition{0};
    std::atomic<int64_t> cancelledShares{0};

public:
    NoiseTrader(OrderEntryGateway& gateway,
                uint64_t referencePrice,
                uint64_t aggression,
                int16_t minQuantity,
                int16_t maxQuantity,
                uint32_t seed);

    bool createOrder() override;
    void getNotification(const Event& event) override;
    void receiveQuoteUpdate(QuoteSummary summary) override;

    uint64_t getOrdersSent() const;
    uint64_t getFills() const;
    int64_t getSharesTraded() const;
    int64_t getNetPosition() const;
    int64_t getCancelledShares() const;

    ~NoiseTrader() override = default;
};
