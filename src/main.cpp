#include <iostream>
#include <memory>
#include <vector>

#include "../include/client/market_maker.h"
#include "../include/client/noise_trader.h"
#include "../include/core/order_book.h"
#include "../include/queue/event_queue.h"
#include "../include/services/order_entry_gateway.h"
#include "../include/services/trade_notification_service.h"
#include "../include/services/retransmission_service.h"

int main() {
    const uint64_t referencePrice = 10000;
    const int noiseTraderCount = 4;
    const int rounds = 50;

    std::cout << "Initializing exchange..." << std::endl;

    EventQueue eventQueue;

    auto notificationService = std::make_unique<TradeNotificationService>();
    notificationService->setEventQueue(&eventQueue);

    OrderBookPool orderBookPool(1, eventQueue);

    RetransmissionService& retransmissionService = RetransmissionService::getInstance();
    retransmissionService.addPoolConsumers(orderBookPool);
    retransmissionService.start();

    OrderEntryGateway orderEntryGateway(orderBookPool);
    orderEntryGateway.start();

    std::vector<std::unique_ptr<NoiseTrader>> noiseTraders;
    for (int i = 0; i < noiseTraderCount; i++) {
        noiseTraders.push_back(std::make_unique<NoiseTrader>(
            orderEntryGateway, referencePrice, 10, 1, 40, static_cast<uint32_t>(1000 + i)));
        notificationService->addClient(noiseTraders.back().get());
    }

    auto marketMaker = std::make_unique<MarketMaker>(orderEntryGateway, referencePrice, 5, 50, 400);
    notificationService->addClient(marketMaker.get());

    notificationService->start();
    TradeNotificationService* notificationPtr = notificationService.get();
    eventQueue.addConsumer(std::move(notificationService));

    std::cout << "Running " << rounds << " rounds: " << noiseTraderCount
              << " noise traders + 1 market maker at reference price " << referencePrice << std::endl;

    for (int round = 0; round < rounds; round++) {
        for (auto& trader : noiseTraders) {
            trader->createOrder();
        }
        marketMaker->createOrder();
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    orderEntryGateway.stop();
    retransmissionService.stop();
    notificationPtr->stop();

    std::cout << "\n=== Simulation complete ===" << std::endl;
    int64_t netPosition = 0;
    int64_t sharesTraded = 0;
    for (const auto& trader : noiseTraders) {
        std::cout << "NoiseTrader " << trader->getId()
                  << ": sent " << trader->getOrdersSent()
                  << ", fills " << trader->getFills()
                  << ", shares " << trader->getSharesTraded()
                  << ", position " << trader->getNetPosition()
                  << ", IOC cancelled " << trader->getCancelledShares() << std::endl;
        netPosition += trader->getNetPosition();
        sharesTraded += trader->getSharesTraded();
    }
    std::cout << "MarketMaker " << marketMaker->getId()
              << ": fills " << marketMaker->getFills()
              << ", shares " << marketMaker->getSharesTraded()
              << ", inventory " << marketMaker->getInventory()
              << ", cash " << marketMaker->getCash()
              << ", mark-to-market PnL " << marketMaker->getMarkToMarketPnl() << std::endl;
    netPosition += marketMaker->getInventory();
    sharesTraded += marketMaker->getSharesTraded();

    std::cout << "\nnet position across all participants: " << netPosition
              << " (must be 0), shares reported by both sides: " << sharesTraded
              << " (must be even)" << std::endl;

    std::cout << "\nExchange shutting down." << std::endl;
    return 0;
}
