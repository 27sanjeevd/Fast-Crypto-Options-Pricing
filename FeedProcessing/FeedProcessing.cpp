#include "FeedProcessing.h"
#include <chrono>
#include <iostream>
#include <sstream>

FeedProcessing::FeedProcessing(const std::string &input_socket_path, const std::string &bbo_output_socket_path)
    : bbo_output_socket_path_(bbo_output_socket_path) {

    ipc_receiver_ = std::make_unique<IPC::IPCReceiver>(input_socket_path);
    ipc_sender_ = std::make_unique<IPC::IPCSender>();

    if (!ipc_receiver_->Initialize()) {
        std::cerr << "Failed to initialize IPC receiver for socket: " << input_socket_path << std::endl;
    }
}

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
            ProcessBookSnapshot(msg);
            book_snapshot_count_++;
        }
        break;
    }
    case ExchangeTypes::MessageType::BOOK_DELTA_UPDATE: {
        const auto *msg = ExchangeTypes::CastToMessage<ExchangeTypes::BookDeltaResponse>(buffer);
        if (msg) {
            ProcessBookDelta(msg);
            book_delta_count_++;
        }
        break;
    }
    case ExchangeTypes::MessageType::TRADE: {
        const auto *msg = ExchangeTypes::CastToMessage<ExchangeTypes::TradeResponse>(buffer);
        if (msg) {
            trade_count_++;
        }
        break;
    }
    default:
        std::cout << "Unknown message type" << std::endl;
        break;
    }
}

void FeedProcessing::ProcessBookSnapshot(const ExchangeTypes::BookSnapshotResponse *msg) {
    if (!msg || msg->result.data.empty()) {
        std::cout << "Empty or invalid book snapshot message" << std::endl;
        return;
    }

    const std::string &instrument = msg->result.instrument_name;
    const auto &snapshot_data = msg->result.data[0]; // Assuming single data element

    if (orderbooks_.find(instrument) == orderbooks_.end()) {
        orderbooks_[instrument] = std::make_unique<Orderbook>();
    }

    auto &orderbook = orderbooks_[instrument];

    last_sequence_numbers_[instrument] = snapshot_data.u;

    std::cout << "Processing book snapshot for " << instrument << " with sequence " << snapshot_data.u << std::endl;

    for (const auto &bid : snapshot_data.bids) {
        double price = std::stod(bid.price);
        double volume = std::stod(bid.size);
        orderbook->UpdateBid(price, volume);
    }

    for (const auto &ask : snapshot_data.asks) {
        double price = std::stod(ask.price);
        double volume = std::stod(ask.size);
        orderbook->UpdateAsk(price, volume);
    }

    SendBboUpdate(instrument, snapshot_data.u);
}

void FeedProcessing::ProcessBookDelta(const ExchangeTypes::BookDeltaResponse *msg) {
    if (!msg || msg->result.data.empty()) {
        std::cout << "Empty or invalid book delta message" << std::endl;
        return;
    }

    const std::string &instrument = msg->result.instrument_name;
    const auto &delta_data = msg->result.data[0]; // Assuming single data element

    if (orderbooks_.find(instrument) == orderbooks_.end()) {
        std::cout << "ERROR: Received delta update for " << instrument << " but no orderbook exists. Sequence: " << delta_data.u << std::endl;
        return;
    }

    auto seq_it = last_sequence_numbers_.find(instrument);
    if (seq_it != last_sequence_numbers_.end()) {
        uint64_t expected_pu = seq_it->second;
        if (delta_data.pu != expected_pu) {
            std::cout << "ERROR: Sequence mismatch for " << instrument << ". Expected pu=" << expected_pu << ", received pu=" << delta_data.pu
                      << ", current u=" << delta_data.u << std::endl;
            return;
        }
    }

    auto &orderbook = orderbooks_[instrument];

    last_sequence_numbers_[instrument] = delta_data.u;

    std::cout << "Processing book delta for " << instrument << " with sequence " << delta_data.u << " (previous: " << delta_data.pu << ")" << std::endl;

    for (const auto &bid : delta_data.update.bids) {
        double price = std::stod(bid.price);
        double volume = std::stod(bid.size);
        orderbook->UpdateBid(price, volume);
    }

    for (const auto &ask : delta_data.update.asks) {
        double price = std::stod(ask.price);
        double volume = std::stod(ask.size);
        orderbook->UpdateAsk(price, volume);
    }

    SendBboUpdate(instrument, delta_data.u);
}

void FeedProcessing::SendBboUpdate(const std::string &instrument, uint64_t sequence_number) {
    auto it = orderbooks_.find(instrument);
    if (it == orderbooks_.end()) {
        return;
    }

    auto current_bbo = it->second->ReturnBbo();

    auto last_bbo_it = last_bbo_.find(instrument);
    if (last_bbo_it != last_bbo_.end()) {
        if (last_bbo_it->second.first == current_bbo.first && last_bbo_it->second.second == current_bbo.second) {
            return;
        }
    }

    last_bbo_[instrument] = current_bbo;

    OrderbookTypes::BboUpdate bbo_update(
        instrument,
        current_bbo.first,  // best_bid
        current_bbo.second, // best_ask
        sequence_number, std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());

    // Connect to BBO output socket if not already connected
    if (!ipc_sender_->IsConnected()) {
        if (!ipc_sender_->Connect(bbo_output_socket_path_)) {
            std::cerr << "Failed to connect BBO sender to: " << bbo_output_socket_path_ << std::endl;
            return;
        }
    }

    if (ipc_sender_->SendData(&bbo_update, sizeof(bbo_update))) {
        std::cout << "BBO UPDATE SENT: " << instrument << " bid=" << current_bbo.first << " ask=" << current_bbo.second << " seq=" << sequence_number
                  << std::endl;
    } else {
        std::cerr << "Failed to send BBO update for " << instrument << std::endl;
    }
}