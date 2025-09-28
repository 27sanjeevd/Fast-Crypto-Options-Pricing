#pragma once

#include "SharedTypes.h"
#include <string>
#include <vector>

namespace ExchangeTypes {

struct TradeData {
    std::string d; // Trade ID
    uint64_t t;    // Trade timestamp in milliseconds
    std::string q; // Trade quantity
    std::string p; // Trade price
    std::string s; // Side (BUY or SELL)
    std::string i; // Instrument name
};

struct TradeResult {
    std::string instrument_name;
    std::string subscription;
    std::string channel;
    std::vector<TradeData> data;
};

struct TradeResponse : public BaseResponse {
    TradeResult result;
};

} // namespace ExchangeTypes
