#pragma once

#include "Data.h"
#include <string>

namespace OrderbookTypes {

struct BboUpdate {
    std::string instrument_name;
    Price best_bid;
    Price best_ask;
    uint64_t sequence_number;
    uint64_t timestamp;

    BboUpdate() = default;
    BboUpdate(const std::string &instrument, Price bid, Price ask, uint64_t seq, uint64_t ts)
        : instrument_name(instrument), best_bid(bid), best_ask(ask), sequence_number(seq), timestamp(ts) {}
};

} // namespace OrderbookTypes
