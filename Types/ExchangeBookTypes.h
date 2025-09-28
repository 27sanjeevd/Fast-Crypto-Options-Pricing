#pragma once

#include <optional>
#include <string>
#include <vector>

namespace ExchangeTypes {

// MessageHeader definition (moved from SharedTypes.h to avoid circular dependency)
enum class MessageType : uint32_t { BOOK_SNAPSHOT = 1, BOOK_DELTA_UPDATE = 2, TRADE = 3, UNKNOWN = 0 };

struct MessageHeader {
    MessageType type;
    uint32_t size; // Total size of the message including header
};

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

struct BaseResponse {
    MessageHeader header; // Must be first field
    int64_t id;
    std::string method;
    int code;
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
