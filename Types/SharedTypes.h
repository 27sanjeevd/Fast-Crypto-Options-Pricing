#pragma once

#include "ExchangeBookTypes.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ExchangeTypes {

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

// Helper functions to cast buffer to message types
template <typename T> const T *CastToMessage(const MessageBuffer &buffer) {
    if (buffer.size() < sizeof(MessageHeader)) {
        return nullptr;
    }
    const MessageHeader *header = reinterpret_cast<const MessageHeader *>(buffer.data());
    if (buffer.size() < header->size) {
        return nullptr;
    }
    return reinterpret_cast<const T *>(buffer.data());
}

template <typename T> T *CastToMessage(MessageBuffer &buffer) {
    if (buffer.size() < sizeof(MessageHeader)) {
        return nullptr;
    }
    const MessageHeader *header = reinterpret_cast<const MessageHeader *>(buffer.data());
    if (buffer.size() < header->size) {
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
