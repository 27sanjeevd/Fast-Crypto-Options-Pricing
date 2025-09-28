#pragma once

#include "SharedTypes.h"
#include <string>
#include <vector>

namespace ExchangeTypes {

struct TradeData {
    std::string d;  // Trade ID
    uint64_t t;     // Trade timestamp in milliseconds
    std::string tn; // Trade timestamp in nanoseconds (optional)
    std::string q;  // Trade quantity
    std::string p;  // Trade price
    std::string s;  // Side (BUY or SELL)
    std::string i;  // Instrument name
};

struct TradeResult {
    std::string instrument_name;
    std::string subscription;
    std::string channel;
    std::vector<TradeData> data;
};

struct TradeResponse {
    MessageHeader header; // Must be first field
    int64_t id;
    std::string method;
    int code;
    TradeResult result;
};

} // namespace ExchangeTypes
