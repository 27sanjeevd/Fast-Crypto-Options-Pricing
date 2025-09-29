#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "DataNormalization.h"
#include "IPCConnection/IPCReceiver.h"
#include "IPCConnection/IPCSender.h"
#include "Orderbook/Orderbook.h"
#include "Orderbook/OrderbookTypes.h"
#include "Types/ExchangeBookTypes.h"
#include "Types/SharedTypes.h"

class FeedProcessing {
  public:
    explicit FeedProcessing(const std::string &data_input_socket_path, const std::string &bbo_output_socket_path = "/tmp/default_bbo_output.sock");
    ~FeedProcessing() = default;

    void Run(std::optional<size_t> max_messages = std::nullopt);

    uint64_t GetBookSnapshotCount() const { return book_snapshot_count_; }
    uint64_t GetBookDeltaCount() const { return book_delta_count_; }
    uint64_t GetTradeCount() const { return trade_count_; }

  private:
    void ProcessMessage(const std::string &message);
    void ProcessBookSnapshot(const ExchangeTypes::BookSnapshotResponse *msg);
    void ProcessBookDelta(const ExchangeTypes::BookDeltaResponse *msg);
    void SendBboUpdate(const std::string &instrument, uint64_t sequence_number);

    std::unique_ptr<IPC::IPCReceiver> ipc_receiver_;
    std::unique_ptr<IPC::IPCSender> ipc_sender_;
    std::string bbo_output_socket_path_;
    DataNormalization data_normalizer_;

    std::unordered_map<std::string, std::unique_ptr<Orderbook>> orderbooks_;
    std::unordered_map<std::string, uint64_t> last_sequence_numbers_;
    std::unordered_map<std::string, std::pair<Price, Price>> last_bbo_;

    uint64_t book_snapshot_count_ = 0;
    uint64_t book_delta_count_ = 0;
    uint64_t trade_count_ = 0;
};
