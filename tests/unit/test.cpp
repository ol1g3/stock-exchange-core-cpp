#include <gtest/gtest.h>
#include "../../include/core/price_level.h"
#include "../../include/common/message.h"

// Test fixture (optional)
class PriceLevelTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code run before each test
    }
    
    void TearDown() override {
        // Cleanup code run after each test
    }
    
    // Shared objects for tests
    PriceLevel priceLevel;
};

// Simple test without fixture
TEST(PriceLevelBasic, EmptyWhenCreated) {
    PriceLevel level;
    EXPECT_TRUE(level.empty());
    EXPECT_EQ(level.size(), 0);
}

// Test using the fixture
TEST_F(PriceLevelTest, AddOrderIncreasesSize) {
    SystemProtocol order;
    order.userId = 1;
    order.price = 100;
    order.quantity = 10;
    order.transaction_id = 123;
    
    priceLevel.addOrder(order);
    
    EXPECT_FALSE(priceLevel.empty());
    EXPECT_EQ(priceLevel.size(), 1);
}