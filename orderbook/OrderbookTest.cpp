#include "Orderbook.h"
#include <gtest/gtest.h>

TEST(OrderbookTest, AddOneBidAndAsk) {
    Orderbook ob;
    ob.update_bid(100.0, 5);
    ob.update_ask(101.0, 3);
    auto bbo = ob.return_bbo();

    EXPECT_EQ(bbo.first, 100.0);
    EXPECT_EQ(bbo.second, 101.0);
}

TEST(OrderbookTest, AddTwoBids) {
    Orderbook ob;
    ob.update_bid(100.0, 5);
    ob.update_bid(101.0, 3);
    auto bbo = ob.return_bbo();

    EXPECT_EQ(bbo.first, 101.0);
}

TEST(OrderbookTest, AddTwoAsks) {
    Orderbook ob;
    ob.update_ask(100.0, 5);
    ob.update_ask(101.0, 3);
    auto bbo = ob.return_bbo();

    EXPECT_EQ(bbo.second, 100.0);
}

TEST(OrderbookTest, UpdateBidVolume) {
    Orderbook ob;
    ob.update_bid(100.0, 5);
    ob.update_bid(100.0, 10);
    auto bbo = ob.return_bbo();

    EXPECT_EQ(bbo.first, 100.0);
}

TEST(OrderbookTest, UpdateAskVolume) {
    Orderbook ob;
    ob.update_ask(100.0, 5);
    ob.update_ask(100.0, 10);
    auto bbo = ob.return_bbo();

    EXPECT_EQ(bbo.second, 100.0);
}

TEST(OrderbookTest, RemoveBid) {
    Orderbook ob;
    ob.update_bid(100.0, 5);
    ob.update_bid(99.0, 5);
    ob.update_bid(100.0, 0);
    auto bbo = ob.return_bbo();

    EXPECT_EQ(bbo.first, 99.0);
}

TEST(OrderbookTest, RemoveAsk) {
    Orderbook ob;
    ob.update_ask(100.0, 5);
    ob.update_ask(101.0, 5);
    ob.update_ask(100.0, 0);
    auto bbo = ob.return_bbo();

    EXPECT_EQ(bbo.second, 101.0);
}