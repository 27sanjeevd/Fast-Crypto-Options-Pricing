#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Types/ExchangeBookTypes.h"
#include "Types/SharedTypes.h"
#include "simdjson.h"

class DataNormalization {
  public:
    DataNormalization();
    ~DataNormalization() = default;

    // Main parsing methods
    std::optional<ExchangeTypes::ExchangeMessage> ParseExchangeMessage(const std::string &json_str);

    // Legacy method for backward compatibility
    std::optional<ExchangeTypes::BookSnapshotResponse> ParseOrderBookUpdate(const std::string &json_str);

  private:
    // Helper methods for parsing different message components
    ExchangeTypes::MessageType DetermineMessageType(const simdjson::dom::object &root);
    ExchangeTypes::Level ParseLevel(const simdjson::dom::array &level_array);
    std::vector<ExchangeTypes::Level> ParseLevels(const simdjson::dom::array &levels_array);
    ExchangeTypes::BookSnapshotData ParseBookSnapshotData(const simdjson::dom::object &data_obj);
    ExchangeTypes::BookDeltaData ParseBookDeltaData(const simdjson::dom::object &data_obj);

    simdjson::dom::parser parser_;
};
