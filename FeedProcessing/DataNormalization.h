#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Types/ExchangeBookTypes.h"
#include "Types/SharedTypes.h"
#include "Types/TradeTypes.h"
#include "simdjson.h"

class DataNormalization {
  public:
    DataNormalization();
    ~DataNormalization() = default;

    ExchangeTypes::MessageBuffer ParseExchangeMessage(const std::string &json_str);

  private:
    // Helper methods for parsing different message components
    ExchangeTypes::MessageType DetermineMessageType(const simdjson::dom::object &root);
    ExchangeTypes::Level ParseLevel(const simdjson::dom::array &level_array);
    std::vector<ExchangeTypes::Level> ParseLevels(const simdjson::dom::array &levels_array);
    ExchangeTypes::BookSnapshotData ParseBookSnapshotData(const simdjson::dom::object &data_obj);
    ExchangeTypes::BookDeltaData ParseBookDeltaData(const simdjson::dom::object &data_obj);

    // Helper methods for parsing different message types
    ExchangeTypes::MessageBuffer ParseBookSnapshotMessage(const simdjson::dom::object &root);
    ExchangeTypes::MessageBuffer ParseBookDeltaMessage(const simdjson::dom::object &root);
    ExchangeTypes::MessageBuffer ParseTradeMessage(const simdjson::dom::object &root);

    // Trade parsing helpers
    ExchangeTypes::TradeData ParseTradeData(const simdjson::dom::object &data_obj);

    // Buffer serialization helpers
    template <typename T> ExchangeTypes::MessageBuffer SerializeMessage(const T &message, ExchangeTypes::MessageType type);

    simdjson::dom::parser parser_;
};
