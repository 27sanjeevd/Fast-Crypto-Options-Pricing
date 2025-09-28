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

  private:
    void ProcessMessage(const std::string &message);

    std::string data_channel_;
    DataNormalization data_normalizer_;
};
