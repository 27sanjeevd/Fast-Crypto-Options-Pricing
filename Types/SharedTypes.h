#pragma once

#include "ExchangeBookTypes.h"
#include <optional>

namespace ExchangeTypes {

// Enum for message types
enum class MessageType { BOOK_SNAPSHOT, BOOK_DELTA_UPDATE, SUBSCRIPTION_REQUEST, UNKNOWN };

// Union-like structure to hold different message types
struct ExchangeMessage {
    MessageType type;

    // Use optional to hold different message types
    std::optional<BookSnapshotResponse> book_snapshot;
    std::optional<BookDeltaResponse> book_delta;
    std::optional<SubscriptionRequest> subscription_request;

    ExchangeMessage(MessageType t) : type(t) {}
};

} // namespace ExchangeTypes
