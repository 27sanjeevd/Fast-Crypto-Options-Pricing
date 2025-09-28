#include "SharedTypes.h"
#include <sstream>

namespace ExchangeTypes {

std::string SubscriptionRequest::to_string() const {
    std::ostringstream oss;

    oss << "{";
    oss << "\"id\":" << id << ",";
    oss << "\"method\":\"" << method << "\",";
    oss << "\"params\":{";

    oss << "\"channels\":[";
    for (size_t i = 0; i < params.channels.size(); ++i) {
        if (i > 0)
            oss << ",";
        oss << "\"" << params.channels[i] << "\"";
    }
    oss << "]";

    if (!params.book_subscription_type.empty()) {
        oss << ",\"book_subscription_type\":\"" << params.book_subscription_type << "\"";
    }

    if (params.book_update_frequency > 0) {
        oss << ",\"book_update_frequency\":" << params.book_update_frequency;
    }

    oss << "}";
    oss << "}";

    return oss.str();
}

} // namespace ExchangeTypes
