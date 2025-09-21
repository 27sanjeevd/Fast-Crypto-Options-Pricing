#include "FeedProcessing.h"
#include <iostream>

FeedProcessing::FeedProcessing(const std::string &data_channel) : data_channel_(data_channel) {}

void FeedProcessing::Run(std::optional<size_t> max_messages) {
    size_t message_count = 0;
    while (true) {
        /*
            Read data from UDS Socket
        */

        std::string message =
            R"({"id":-1,"method":"subscribe","code":0,"result":{"instrument_name":"ETHUSD-PERP","subscription":"book.ETHUSD-PERP.10","channel":"book.update","depth":10,"data":[{"update":{"asks":[["4457.07","4.4444","8"]],"bids":[]},"t":1758447954211,"tt":1758447954207,"u":249977186042272,"pu":249977185983904,"cs":590576900}]}})";
        ProcessMessage(message);

        message_count++;
        if (max_messages.has_value() && message_count >= max_messages.value()) {
            break;
        }
    }
}

void FeedProcessing::ProcessMessage(const std::string &message) {
    auto exchange_message = ParseExchangeMessage(message);
    if (exchange_message.has_value()) {
        switch (exchange_message->type) {
        case ExchangeTypes::MessageType::BOOK_SNAPSHOT:
            if (exchange_message->book_snapshot.has_value()) {
                auto &snapshot = exchange_message->book_snapshot.value();
                std::cout << "Parsed book snapshot for " << snapshot.result.instrument_name << " with depth " << snapshot.result.depth << std::endl;
            }
            break;
        case ExchangeTypes::MessageType::BOOK_DELTA_UPDATE:
            if (exchange_message->book_delta.has_value()) {
                auto &delta = exchange_message->book_delta.value();
                std::cout << "Parsed book delta update for " << delta.result.instrument_name << " with depth " << delta.result.depth << std::endl;
            }
            break;
        case ExchangeTypes::MessageType::SUBSCRIPTION_REQUEST:
            if (exchange_message->subscription_request.has_value()) {
                auto &request = exchange_message->subscription_request.value();
                std::cout << "Parsed subscription request with " << request.params.channels.size() << " channels" << std::endl;
            }
            break;
        default:
            std::cout << "Unknown message type" << std::endl;
            break;
        }

        // Forward to orderbook or other components
        // orderbook_->UpdateOrderBook(*exchange_message);
    } else {
        std::cerr << "Failed to parse message: " << message << std::endl;
    }
}

std::optional<ExchangeTypes::ExchangeMessage> FeedProcessing::ParseExchangeMessage(const std::string &json_str) {
    return data_normalizer_.ParseExchangeMessage(json_str);
}

// Legacy method for backward compatibility
std::optional<ExchangeTypes::BookSnapshotResponse> FeedProcessing::ParseOrderBookUpdate(const std::string &json_str) {
    return data_normalizer_.ParseOrderBookUpdate(json_str);
}