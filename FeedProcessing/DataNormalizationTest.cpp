#include "DataNormalization.h"
#include <gtest/gtest.h>

TEST(DataNormalizationTest, ParseBookDeltaMessage) {
    DataNormalization normalizer;
    std::string json_str =
        R"(
            {
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
                            "asks":[
                                ["50126.000000", "0", "0"],
                                ["50180.000000", "3.279000", "10"]
                            ],
                            "bids":[
                                ["50097.000000", "0.252000", "1"]
                            ]
                        },
                        "tt": 1647917463003,
                        "t": 1647917463003,
                        "u": 7845460002,
                        "pu": 7845460001
                    }]
                }
            }        
        )";

    auto result = normalizer.ParseExchangeMessage(json_str);

    EXPECT_EQ(ExchangeTypes::GetMessageType(result), ExchangeTypes::MessageType::BOOK_DELTA_UPDATE);

    auto delta = ExchangeTypes::CastToMessage<ExchangeTypes::BookDeltaResponse>(result);
    EXPECT_EQ(delta->result.instrument_name, "BTCUSD-PERP");
    EXPECT_EQ(delta->result.depth, 10);
    EXPECT_EQ(delta->result.channel, "book.update");

    auto &data = delta->result.data[0];
    EXPECT_EQ(data.update.asks.size(), 2);
    EXPECT_EQ(data.update.bids.size(), 1);
    EXPECT_EQ(data.tt, 1647917463003);
    EXPECT_EQ(data.t, 1647917463003);
    EXPECT_EQ(data.u, 7845460002);
    EXPECT_EQ(data.pu, 7845460001);
}

TEST(DataNormalizationTest, ParseBookSnapshotMessage) {
    DataNormalization normalizer;
    std::string json_str =
        R"(
            {
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
                            ["50126.000000", "0.400000", "2"],
                            ["50130.000000", "1.279000", "3"],
                            ["50136.000000", "1.279000", "5"],
                            ["50137.000000", "0.800000", "7"],
                            ["50142.000000", "1.279000", "1"],
                            ["50148.000000", "2.892900", "9"],
                            ["50154.000000", "1.279000", "5"],
                            ["50160.000000", "1.133000", "2"],
                            ["50166.000000", "3.090700", "1"],
                            ["50172.000000", "1.279000", "1"]
                        ],
                        "bids": [
                            ["50113.500000", "0.400000", "3"],
                            ["50113.000000", "0.051800", "1"],
                            ["50112.000000", "1.455300", "1"],
                            ["50106.000000", "1.174800", "2"],
                            ["50100.500000", "0.800000", "4"],
                            ["50100.000000", "1.455300", "5"],
                            ["50097.500000", "0.048000", "8"],
                            ["50097.000000", "0.148000", "9"],
                            ["50096.500000", "0.399200", "2"],
                            ["50095.000000", "0.399200", "3"]
                        ],
                        "tt": 1647917462799,
                        "t": 1647917463000,
                        "u": 7845460001
                    }]
                }
            }
        )";

    auto result = normalizer.ParseExchangeMessage(json_str);

    EXPECT_EQ(ExchangeTypes::GetMessageType(result), ExchangeTypes::MessageType::BOOK_SNAPSHOT);

    auto snapshot = ExchangeTypes::CastToMessage<ExchangeTypes::BookSnapshotResponse>(result);
    EXPECT_EQ(snapshot->result.instrument_name, "BTCUSD-PERP");
    EXPECT_EQ(snapshot->result.depth, 10);
    EXPECT_EQ(snapshot->result.channel, "book");

    auto &data = snapshot->result.data[0];
    EXPECT_EQ(data.tt, 1647917462799);
    EXPECT_EQ(data.t, 1647917463000);
    EXPECT_EQ(data.u, 7845460001);
}

TEST(DataNormalizationTest, ParseTradeMessage) {
    DataNormalization normalizer;
    std::string json_str =
        R"(
            {
                "id": 1,
                "method": "subscribe",
                "code": 0,
                "result": {
                    "instrument_name": "BTCUSD-PERP",
                    "subscription": "trade.BTCUSD-PERP",
                    "channel": "trade",
                    "data": [{
                        "d" : "2030407068",
                        "t": 1613581138462,
                        "p": "51327.500000",
                        "q": "0.000100",
                        "s": "SELL",
                        "i": "BTCUSD-PERP"
                    }]
                }
            }
        )";

    auto result = normalizer.ParseExchangeMessage(json_str);

    EXPECT_EQ(ExchangeTypes::GetMessageType(result), ExchangeTypes::MessageType::TRADE);

    auto trade = ExchangeTypes::CastToMessage<ExchangeTypes::TradeResponse>(result);
    EXPECT_EQ(trade->result.instrument_name, "BTCUSD-PERP");
    EXPECT_EQ(trade->result.subscription, "trade.BTCUSD-PERP");
    EXPECT_EQ(trade->result.channel, "trade");

    auto &data = trade->result.data[0];
    EXPECT_EQ(data.d, "2030407068");
    EXPECT_EQ(data.t, 1613581138462);
    EXPECT_EQ(data.p, "51327.500000");
    EXPECT_EQ(data.q, "0.000100");
    EXPECT_EQ(data.s, "SELL");
    EXPECT_EQ(data.i, "BTCUSD-PERP");
}

TEST(DataNormalizationTest, ParseInvalidMessage) {
    DataNormalization normalizer;
    std::string json_str = "invalid json";

    auto result = normalizer.ParseExchangeMessage(json_str);

    EXPECT_EQ(ExchangeTypes::GetMessageType(result), ExchangeTypes::MessageType::UNKNOWN);
}
