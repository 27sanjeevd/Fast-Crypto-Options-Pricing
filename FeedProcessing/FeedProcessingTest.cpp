#include "FeedProcessing.h"
#include "IPCConnection/IPCSender.h"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

TEST(FeedProcessingTest, ParseValidOrderBookUpdate) {
    FeedProcessing fp("book.ETHUSD-PERP.10");

    std::thread fp_thread([&fp]() { fp.Run(3); });

    // Initial Delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    IPC::IPCSender sender;
    EXPECT_TRUE(sender.Connect(fp.GetSocketPath()));

    std::string message1 = R"({
        "id": -1,
        "method": "subscribe",
        "code": 0,
        "result": {
            "instrument_name": "ETHUSD-PERP",
            "subscription": "book.ETHUSD-PERP.10",
            "channel": "book",
            "depth": 10,
            "data": [{
                "asks": [
                    ["2500.00", "1.0", "1"],
                    ["2501.00", "2.0", "2"]
                ],
                "bids": [
                    ["2499.00", "1.5", "1"],
                    ["2498.00", "0.5", "1"]
                ],
                "tt": 1647917462799,
                "t": 1647917463000,
                "u": 7845460001
            }]
        }
    })";

    std::string message2 = R"({
        "id": -1,
        "method": "subscribe",
        "code": 0,
        "result": {
            "instrument_name": "ETHUSD-PERP",
            "subscription": "book.ETHUSD-PERP.10",
            "channel": "book",
            "depth": 10,
            "data": [{
                "asks": [
                    ["2500.50", "0.8", "1"],
                    ["2501.50", "1.2", "1"]
                ],
                "bids": [
                    ["2499.50", "2.0", "1"],
                    ["2498.50", "1.0", "1"]
                ],
                "tt": 1647917462800,
                "t": 1647917463001,
                "u": 7845460002
            }]
        }
    })";

    std::string message3 = R"({
        "id": -1,
        "method": "subscribe",
        "code": 0,
        "result": {
            "instrument_name": "ETHUSD-PERP",
            "subscription": "book.ETHUSD-PERP.10",
            "channel": "book",
            "depth": 10,
            "data": [{
                "asks": [
                    ["2501.00", "0.5", "1"],
                    ["2502.00", "1.8", "2"]
                ],
                "bids": [
                    ["2500.00", "1.2", "1"],
                    ["2499.00", "0.8", "1"]
                ],
                "tt": 1647917462801,
                "t": 1647917463002,
                "u": 7845460003
            }]
        }
    })";

    EXPECT_TRUE(sender.SendData(message1.c_str(), message1.size()));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    EXPECT_TRUE(sender.SendData(message2.c_str(), message2.size()));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    EXPECT_TRUE(sender.SendData(message3.c_str(), message3.size()));

    fp_thread.join();

    EXPECT_EQ(fp.GetBookSnapshotCount(), 3);
    EXPECT_EQ(fp.GetBookDeltaCount(), 0);
    EXPECT_EQ(fp.GetTradeCount(), 0);
}

TEST(FeedProcessingTest, ParseMixedMessages) {
    FeedProcessing fp("book.BTCUSD-PERP.10");

    std::thread fp_thread([&fp]() { fp.Run(3); });

    // Initial Delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    IPC::IPCSender sender;
    EXPECT_TRUE(sender.Connect(fp.GetSocketPath()));

    std::string message1 =
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

    EXPECT_TRUE(sender.SendData(message1.c_str(), message1.size()));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::string message2 =
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

    EXPECT_TRUE(sender.SendData(message2.c_str(), message2.size()));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::string message3 =
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

    EXPECT_TRUE(sender.SendData(message3.c_str(), message3.size()));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    fp_thread.join();

    EXPECT_EQ(fp.GetBookSnapshotCount(), 1);
    EXPECT_EQ(fp.GetBookDeltaCount(), 1);
    EXPECT_EQ(fp.GetTradeCount(), 1);
}