#include "FeedProcessing.h"
#include <iostream>

FeedProcessing::FeedProcessing(const std::string &data_channel) : data_channel_(data_channel) {}

void FeedProcessing::Run(std::optional<size_t> max_messages) {
    size_t message_count = 0;
    while (true) {
        /*
            Read data from UDS Socket
        */

        // Temporary definition until we setup socket reading
        std::string message =
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

        ProcessMessage(message);

        message_count++;
        if (max_messages.has_value() && message_count >= max_messages.value()) {
            break;
        }
    }
}

void FeedProcessing::ProcessMessage(const std::string &message) {
    auto buffer = data_normalizer_.ParseExchangeMessage(message);

    if (buffer.empty()) {
        std::cout << "Failed to parse message" << std::endl;
        return;
    }

    ExchangeTypes::MessageType msg_type = ExchangeTypes::GetMessageType(buffer);

    switch (msg_type) {
    case ExchangeTypes::MessageType::BOOK_SNAPSHOT: {
        const auto *msg = ExchangeTypes::CastToMessage<ExchangeTypes::BookSnapshotResponse>(buffer);
        if (msg) {
            std::cout << "Parsed book snapshot for " << msg->result.instrument_name << " with depth " << msg->result.depth << std::endl;
        }
        break;
    }
    case ExchangeTypes::MessageType::BOOK_DELTA_UPDATE: {
        const auto *msg = ExchangeTypes::CastToMessage<ExchangeTypes::BookDeltaResponse>(buffer);
        if (msg) {
            std::cout << "Parsed book delta update for " << msg->result.instrument_name << " with depth " << msg->result.depth << std::endl;
        }
        break;
    }
    case ExchangeTypes::MessageType::TRADE: {
        const auto *msg = ExchangeTypes::CastToMessage<ExchangeTypes::TradeResponse>(buffer);
        if (msg) {
            std::cout << "Parsed trade message for " << msg->result.instrument_name << " with " << msg->result.data.size() << " trades" << std::endl;
        }
        break;
    }
    default:
        std::cout << "Unknown message type" << std::endl;
        break;
    }
}