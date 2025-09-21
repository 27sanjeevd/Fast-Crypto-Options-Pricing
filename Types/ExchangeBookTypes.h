#pragma once

#include <optional>
#include <string>
#include <vector>

namespace ExchangeTypes {

// Represents a single level in the order book [price, size, num_orders]
struct Level {
    std::string price;      // Price as string to maintain precision
    std::string size;       // Size as string to maintain precision
    std::string num_orders; // Number of orders as string

    Level(const std::string &p, const std::string &s, const std::string &n) : price(p), size(s), num_orders(n) {}

    Level() = default;
};

// Represents the book update data for delta updates
struct BookUpdateData {
    std::vector<Level> asks;
    std::vector<Level> bids;
};

// Represents the book snapshot data
struct BookSnapshotData {
    std::vector<Level> asks;
    std::vector<Level> bids;
    uint64_t tt; // Epoch millis of last book update
    uint64_t t;  // Epoch millis of message publish
    uint64_t u;  // Update sequence
};

// Represents the book delta update data
struct BookDeltaData {
    BookUpdateData update;
    uint64_t tt; // Epoch millis of last book update
    uint64_t t;  // Epoch millis of message publish
    uint64_t u;  // Update sequence
    uint64_t pu; // Previous update sequence
};

// Base response structure
struct BaseResponse {
    int64_t id;
    std::string method;
    int code;
};

// Subscription response result for book snapshot
struct BookSnapshotResult {
    std::string instrument_name;
    std::string subscription;
    std::string channel;
    int depth;
    std::vector<BookSnapshotData> data;
};

// Subscription response result for book delta updates
struct BookDeltaResult {
    std::string instrument_name;
    std::string subscription;
    std::string channel;
    int depth;
    std::vector<BookDeltaData> data;
};

// Complete response structures
struct BookSnapshotResponse : public BaseResponse {
    BookSnapshotResult result;
};

struct BookDeltaResponse : public BaseResponse {
    BookDeltaResult result;
};

// Subscription request structure
struct SubscriptionParams {
    std::vector<std::string> channels;
    std::optional<std::string> book_subscription_type; // "SNAPSHOT" or "SNAPSHOT_AND_UPDATE"
    std::optional<int> book_update_frequency;          // 10, 100, or 500
};

struct SubscriptionRequest {
    int64_t id;
    std::string method;
    SubscriptionParams params;
};

} // namespace ExchangeTypes
