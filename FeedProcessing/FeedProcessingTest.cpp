#include "FeedProcessing.h"
#include <gtest/gtest.h>

TEST(FeedProcessingTest, ParseValidOrderBookUpdate) {
    FeedProcessing fp("book.ETHUSD-PERP.10");
    std::string message =
        R"({"id":-1,"method":"subscribe","code":0,"result":{"instrument_name":"ETHUSD-PERP","subscription":"book.ETHUSD-PERP.10","channel":"book.update","depth":10,"data":[{"update":{"asks":[["4457.07","4.4444","8"]],"bids":[["4456.50","2.0","5"]]},"t":1758447954211,"tt":1758447954207,"u":249977186042272,"pu":249977185983904,"cs":590576900}]}})";

    auto result = fp.ParseExchangeMessage(message);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->type, ExchangeTypes::MessageType::BOOK_DELTA_UPDATE);
    ASSERT_TRUE(result->book_delta.has_value());

    auto &delta = result->book_delta.value();
    EXPECT_EQ(delta.result.instrument_name, "ETHUSD-PERP");
    EXPECT_EQ(delta.result.channel, "book.update");
    EXPECT_EQ(delta.result.depth, 10);
    ASSERT_FALSE(delta.result.data.empty());

    auto &data = delta.result.data[0];
    EXPECT_EQ(data.t, 1758447954211);
    EXPECT_EQ(data.u, 249977186042272);
    EXPECT_EQ(data.pu, 249977185983904);
    ASSERT_EQ(data.update.asks.size(), 1);
    ASSERT_EQ(data.update.bids.size(), 1);
    EXPECT_EQ(data.update.asks[0].price, "4457.07");
    EXPECT_EQ(data.update.asks[0].size, "4.4444");
    EXPECT_EQ(data.update.asks[0].num_orders, "8");
    EXPECT_EQ(data.update.bids[0].price, "4456.50");
    EXPECT_EQ(data.update.bids[0].size, "2.0");
    EXPECT_EQ(data.update.bids[0].num_orders, "5");
}

TEST(FeedProcessingTest, ParseBookSnapshotMessage) {
    FeedProcessing fp("book.BTCUSD-PERP.10");

    std::string snapshot_message = R"({
        "id": -1,
        "method": "subscribe",
        "code": 0,
        "result": {
            "instrument_name": "BTCUSD-PERP",
            "subscription": "book.BTCUSD-PERP.10",
            "channel": "book",
            "depth": 10,
            "data": [{
                "asks": [
                    ["30082.5", "0.1689", "1"],
                    ["30083.0", "0.1288", "1"],
                    ["30084.5", "0.0171", "1"]
                ],
                "bids": [
                    ["30079.0", "0.0505", "1"],
                    ["30077.5", "1.0527", "2"],
                    ["30076.0", "0.1689", "1"]
                ],
                "t": 1654780033786,
                "tt": 1654780033755,
                "u": 542048017824
            }]
        }
    })";

    auto result = fp.ParseExchangeMessage(snapshot_message);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->type, ExchangeTypes::MessageType::BOOK_SNAPSHOT);
    ASSERT_TRUE(result->book_snapshot.has_value());

    auto &snapshot = result->book_snapshot.value();
    EXPECT_EQ(snapshot.result.instrument_name, "BTCUSD-PERP");
    EXPECT_EQ(snapshot.result.channel, "book");
    EXPECT_EQ(snapshot.result.depth, 10);
    ASSERT_FALSE(snapshot.result.data.empty());

    auto &data = snapshot.result.data[0];
    EXPECT_EQ(data.t, 1654780033786);
    EXPECT_EQ(data.tt, 1654780033755);
    EXPECT_EQ(data.u, 542048017824);

    ASSERT_EQ(data.asks.size(), 3);
    EXPECT_EQ(data.asks[0].price, "30082.5");
    EXPECT_EQ(data.asks[0].size, "0.1689");
    EXPECT_EQ(data.asks[0].num_orders, "1");

    ASSERT_EQ(data.bids.size(), 3);
    EXPECT_EQ(data.bids[0].price, "30079.0");
    EXPECT_EQ(data.bids[0].size, "0.0505");
    EXPECT_EQ(data.bids[0].num_orders, "1");
}

TEST(FeedProcessingTest, ParseBookDeltaUpdateMessage) {
    FeedProcessing fp("book.BTCUSD-PERP.10");

    std::string delta_message = R"({
        "id": -1,
        "method": "subscribe",
        "code": 0,
        "result": {
            "instrument_name": "BTCUSD-PERP",
            "subscription": "book.BTCUSD-PERP.10",
            "channel": "book.update",
            "depth": 10,
            "data": [{
                "update": {
                    "asks": [
                        ["50126.000000", "0", "0"],
                        ["50180.000000", "3.279000", "10"]
                    ],
                    "bids": [
                        ["50097.000000", "0.252000", "1"]
                    ]
                },
                "tt": 1647917463003,
                "t": 1647917463003,
                "u": 7845460002,
                "pu": 7845460001
            }]
        }
    })";

    auto result = fp.ParseExchangeMessage(delta_message);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->type, ExchangeTypes::MessageType::BOOK_DELTA_UPDATE);
    ASSERT_TRUE(result->book_delta.has_value());

    auto &delta = result->book_delta.value();
    EXPECT_EQ(delta.result.instrument_name, "BTCUSD-PERP");
    EXPECT_EQ(delta.result.channel, "book.update");
    EXPECT_EQ(delta.result.depth, 10);
    ASSERT_FALSE(delta.result.data.empty());

    auto &data = delta.result.data[0];
    EXPECT_EQ(data.t, 1647917463003);
    EXPECT_EQ(data.tt, 1647917463003);
    EXPECT_EQ(data.u, 7845460002);
    EXPECT_EQ(data.pu, 7845460001);

    ASSERT_EQ(data.update.asks.size(), 2);
    EXPECT_EQ(data.update.asks[0].price, "50126.000000");
    EXPECT_EQ(data.update.asks[0].size, "0");
    EXPECT_EQ(data.update.asks[0].num_orders, "0");
    EXPECT_EQ(data.update.asks[1].price, "50180.000000");
    EXPECT_EQ(data.update.asks[1].size, "3.279000");
    EXPECT_EQ(data.update.asks[1].num_orders, "10");

    ASSERT_EQ(data.update.bids.size(), 1);
    EXPECT_EQ(data.update.bids[0].price, "50097.000000");
    EXPECT_EQ(data.update.bids[0].size, "0.252000");
    EXPECT_EQ(data.update.bids[0].num_orders, "1");
}

TEST(FeedProcessingTest, ParseInvalidMessage) {
    FeedProcessing fp("book.BTCUSD-PERP.10");

    std::string invalid_message = R"({"invalid": "json", "structure": true})";

    auto result = fp.ParseExchangeMessage(invalid_message);
    EXPECT_FALSE(result.has_value());
}

TEST(FeedProcessingTest, ParseEmptyMessage) {
    FeedProcessing fp("book.BTCUSD-PERP.10");

    std::string empty_message = "";

    auto result = fp.ParseExchangeMessage(empty_message);
    EXPECT_FALSE(result.has_value());
}