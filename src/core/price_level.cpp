#include "../../include/core/price_level.h"


bool PriceLevel::empty(){
    return ordersBySize.empty() && ordersByTime.empty();
}

void PriceLevel::addOrder(SystemProtocol order) {
    if(comparisonType == OrderComparisonType::SIZE) ordersBySize.insert(&order);
    else ordersByTime.insert(&order);
}

void PriceLevel::removeOrder(SystemProtocol order) {
    if(comparisonType == OrderComparisonType::SIZE) ordersBySize.erase(&order);
    else ordersByTime.erase(&order);
}

void PriceLevel::updateOrder(SystemProtocol order) {
    removeOrder(order);
    addOrder(order);
}

const SystemProtocol* PriceLevel::getBestOrder() {
    if(comparisonType == OrderComparisonType::SIZE) return *ordersBySize.begin();
    else return *ordersByTime.begin();
}
