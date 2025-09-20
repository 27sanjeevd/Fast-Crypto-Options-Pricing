#pragma once

#include <tuple>
#include <vector>

using Price = double;
using Volume = double;

struct ReceivedData {
    uint32_t message_type;
    uint32_t currency_name;
    uint32_t num_levels;
};

struct BookUpdate {
    Price price;
    Volume volume;
};