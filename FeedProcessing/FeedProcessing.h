#pragma once

#include <optional>
#include <string>
#include <vector>

#include "DataNormalization.h"
#include "Types/ExchangeBookTypes.h"
#include "Types/SharedTypes.h"

class FeedProcessing {
  public:
    FeedProcessing(const std::string &data_channel);
    ~FeedProcessing() = default;

    void Run(std::optional<size_t> max_messages = std::nullopt);

    // Parse different types of exchange messages (delegated to DataNormalization)
    std::optional<ExchangeTypes::ExchangeMessage> ParseExchangeMessage(const std::string &json_str);

    // Legacy method for backward compatibility (delegated to DataNormalization)
    std::optional<ExchangeTypes::BookSnapshotResponse> ParseOrderBookUpdate(const std::string &json_str);

  private:
    void ProcessMessage(const std::string &message);

    std::string data_channel_;
    DataNormalization data_normalizer_;
};
