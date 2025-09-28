#pragma once

#include "SharedTypes.h"
#include <optional>
#include <string>
#include <vector>

namespace ExchangeTypes {

struct Level {
    std::string price;
    std::string size;
    std::string num_orders;

    Level(const std::string &p, const std::string &s, const std::string &n) : price(p), size(s), num_orders(n) {}
    Level() = default;
};

struct BookUpdateData {
    std::vector<Level> asks;
    std::vector<Level> bids;
};

struct BookSnapshotData {
    std::vector<Level> asks;
    std::vector<Level> bids;
    uint64_t tt;
    uint64_t t;
    uint64_t u;
};

struct BookDeltaData {
    BookUpdateData update;
    uint64_t tt;
    uint64_t t;
    uint64_t u;
    uint64_t pu;
};

struct BookSnapshotResult {
    std::string instrument_name;
    std::string subscription;
    std::string channel;
    int depth;
    std::vector<BookSnapshotData> data;
};

struct BookDeltaResult {
    std::string instrument_name;
    std::string subscription;
    std::string channel;
    int depth;
    std::vector<BookDeltaData> data;
};

struct BookSnapshotResponse : public BaseResponse {
    BookSnapshotResult result;
};

struct BookDeltaResponse : public BaseResponse {
    BookDeltaResult result;
};

} // namespace ExchangeTypes
