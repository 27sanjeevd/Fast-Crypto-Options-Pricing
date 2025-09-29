#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "DataNormalization.h"
#include "IPCConnection/IPCReceiver.h"
#include "Types/ExchangeBookTypes.h"
#include "Types/SharedTypes.h"

class FeedProcessing {
  public:
    FeedProcessing(const std::string &data_channel);
    ~FeedProcessing() = default;

    void Run(std::optional<size_t> max_messages = std::nullopt);

    const std::string &GetSocketPath() const;

    uint64_t GetBookSnapshotCount() const { return book_snapshot_count_; }
    uint64_t GetBookDeltaCount() const { return book_delta_count_; }
    uint64_t GetTradeCount() const { return trade_count_; }

  private:
    void ProcessMessage(const std::string &message);

    std::string data_channel_;
    std::unique_ptr<IPC::IPCReceiver> ipc_receiver_;
    DataNormalization data_normalizer_;

    uint64_t book_snapshot_count_ = 0;
    uint64_t book_delta_count_ = 0;
    uint64_t trade_count_ = 0;
};
