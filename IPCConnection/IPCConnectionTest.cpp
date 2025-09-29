#include "IPCReceiver.h"
#include "IPCSender.h"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

namespace IPC {

TEST(IPCConnectionTest, BasicSendReceive) {
    IPCReceiver receiver("test_socket");
    EXPECT_TRUE(receiver.Initialize());

    std::vector<uint8_t> received_data;
    bool data_received = false;

    std::thread receiver_thread([&receiver, &received_data, &data_received]() {
        received_data = receiver.ReadData();
        data_received = true;
    });

    // Delay for setup
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    IPCSender sender;
    EXPECT_TRUE(sender.Connect(receiver.GetSocketPath()));

    std::string test_message = "Hello, IPC World!";
    EXPECT_TRUE(sender.SendData(test_message.c_str(), test_message.size()));

    receiver_thread.join();

    EXPECT_TRUE(data_received);
    std::string received_message(received_data.begin(), received_data.end());
    EXPECT_EQ(received_message, test_message);
}

TEST(IPCConnectionTest, BinarySendReceive) {
    IPCReceiver receiver("test_binary_socket");
    EXPECT_TRUE(receiver.Initialize());

    std::vector<uint8_t> received_data;
    bool data_received = false;

    std::thread receiver_thread([&receiver, &received_data, &data_received]() {
        received_data = receiver.ReadData();
        data_received = true;
    });

    // Delay for setup
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    IPCSender sender;
    EXPECT_TRUE(sender.Connect(receiver.GetSocketPath()));

    // Create some test binary data
    std::vector<uint8_t> test_data = {0x01, 0x02, 0x03, 0x04, 0xFF, 0xAB, 0xCD, 0xEF};
    EXPECT_TRUE(sender.SendData(test_data.data(), test_data.size()));

    receiver_thread.join();

    EXPECT_TRUE(data_received);
    EXPECT_EQ(received_data, test_data);
}

TEST(IPCConnectionTest, MultipleMessages) {
    IPCReceiver receiver("test_multi_socket");
    EXPECT_TRUE(receiver.Initialize());

    std::vector<std::string> received_messages;

    std::thread receiver_thread([&receiver, &received_messages]() {
        for (int i = 0; i < 3; i++) {
            auto data = receiver.ReadData();
            if (!data.empty()) {
                std::string msg(data.begin(), data.end());
                received_messages.push_back(msg);
            }
        }
    });

    // Delay for setup
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    IPCSender sender;
    EXPECT_TRUE(sender.Connect(receiver.GetSocketPath()));

    std::vector<std::string> test_messages = {"Message 1", "Message 2", "Message 3"};

    for (const auto &msg : test_messages) {
        EXPECT_TRUE(sender.SendData(msg.c_str(), msg.size()));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    receiver_thread.join();

    EXPECT_EQ(received_messages.size(), 3);
    for (size_t i = 0; i < test_messages.size(); i++) {
        EXPECT_EQ(received_messages[i], test_messages[i]);
    }
}

} // namespace IPC
