#include "FeedProcessing.h"
#include <iostream>

FeedProcessing::FeedProcessing(const std::string &data_channel) : data_channel_(data_channel) {

    ipc_receiver_ = std::make_unique<IPC::IPCReceiver>("feedprocessing_" + data_channel);

    if (!ipc_receiver_->Initialize()) {
        std::cerr << "Failed to initialize IPC receiver for channel: " << data_channel << std::endl;
    }
}

const std::string &FeedProcessing::GetSocketPath() const { return ipc_receiver_->GetSocketPath(); }

void FeedProcessing::Run(std::optional<size_t> max_messages) {
    if (!ipc_receiver_->IsInitialized()) {
        std::cerr << "IPC receiver not initialized, cannot run" << std::endl;
        return;
    }

    size_t message_count = 0;
    while (true) {
        auto data = ipc_receiver_->ReadData();

        if (data.empty()) {
            continue;
        }

        std::string message(data.begin(), data.end());
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