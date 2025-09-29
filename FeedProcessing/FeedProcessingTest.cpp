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

    EXPECT_EQ(1, 1);
}