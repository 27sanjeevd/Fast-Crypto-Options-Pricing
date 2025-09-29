#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ExchangeTypes {

// Message types and header definitions
enum class MessageType : uint32_t { BOOK_SNAPSHOT = 1, BOOK_DELTA_UPDATE = 2, TRADE = 3, UNKNOWN = 0 };

struct MessageHeader {
    MessageType type;
};

using MessageBuffer = std::vector<uint8_t>;

struct SubscriptionParams {
    std::vector<std::string> channels;
    std::string book_subscription_type;
    int book_update_frequency = 0;
};

struct SubscriptionRequest {
    int64_t id;
    std::string method;
    SubscriptionParams params;

    std::string to_string() const;
};

// Base response structure for all message responses
struct BaseResponse {
    MessageHeader header; // Must be first field
    int64_t id;
    std::string method;
    int code;
};

// Helper functions to cast buffer to message types
template <typename T> const T *CastToMessage(const MessageBuffer &buffer) {
    if (buffer.size() < sizeof(MessageHeader)) {
        return nullptr;
    }

    return reinterpret_cast<const T *>(buffer.data());
}

template <typename T> T *CastToMessage(MessageBuffer &buffer) {
    if (buffer.size() < sizeof(MessageHeader)) {
        return nullptr;
    }

    return reinterpret_cast<T *>(buffer.data());
}

inline MessageType GetMessageType(const MessageBuffer &buffer) {
    if (buffer.size() < sizeof(MessageHeader)) {
        return MessageType::UNKNOWN;
    }
    const MessageHeader *header = reinterpret_cast<const MessageHeader *>(buffer.data());
    return header->type;
}

} // namespace ExchangeTypes
