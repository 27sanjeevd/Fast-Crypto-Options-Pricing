#include "DataNormalization.h"
#include <cstring>
#include <iostream>

DataNormalization::DataNormalization() {}

ExchangeTypes::MessageBuffer DataNormalization::ParseExchangeMessage(const std::string &json_str) {
    try {
        simdjson::dom::element doc = parser_.parse(json_str);

        if (doc.type() != simdjson::dom::element_type::OBJECT) {
            return ExchangeTypes::MessageBuffer{};
        }

        simdjson::dom::object root = doc;
        ExchangeTypes::MessageType msg_type = DetermineMessageType(root);

        switch (msg_type) {
        case ExchangeTypes::MessageType::BOOK_SNAPSHOT:
            return ParseBookSnapshotMessage(root);

        case ExchangeTypes::MessageType::BOOK_DELTA_UPDATE:
            return ParseBookDeltaMessage(root);

        case ExchangeTypes::MessageType::TRADE:
            return ParseTradeMessage(root);

        default:
            return ExchangeTypes::MessageBuffer{};
        }

    } catch (const std::exception &e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return ExchangeTypes::MessageBuffer{};
    }
}

ExchangeTypes::MessageType DataNormalization::DetermineMessageType(const simdjson::dom::object &root) {
    auto result = root["result"];
    if (result.error() == simdjson::SUCCESS) {
        std::string_view channel;
        if (result["channel"].get_string().get(channel) == simdjson::SUCCESS) {
            if (channel == "book") {
                return ExchangeTypes::MessageType::BOOK_SNAPSHOT;
            } else if (channel == "book.update") {
                return ExchangeTypes::MessageType::BOOK_DELTA_UPDATE;
            } else if (channel == "trade") {
                return ExchangeTypes::MessageType::TRADE;
            }
        }
    }

    return ExchangeTypes::MessageType::UNKNOWN;
}

template <typename T> ExchangeTypes::MessageBuffer DataNormalization::SerializeMessage(const T &message, ExchangeTypes::MessageType type) {
    ExchangeTypes::MessageBuffer buffer(sizeof(T));

    // Cast the message to bytes and copy
    T *msg_copy = reinterpret_cast<T *>(buffer.data());
    *msg_copy = message;

    // Set the header
    msg_copy->header.type = type;

    return buffer;
}

ExchangeTypes::Level DataNormalization::ParseLevel(const simdjson::dom::array &level_array) {
    ExchangeTypes::Level level;

    if (level_array.size() >= 3) {
        std::string_view price, size, num_orders;

        if (level_array.at(0).get_string().get(price) == simdjson::SUCCESS) {
            level.price = std::string(price);
        }

        if (level_array.at(1).get_string().get(size) == simdjson::SUCCESS) {
            level.size = std::string(size);
        }

        if (level_array.at(2).get_string().get(num_orders) == simdjson::SUCCESS) {
            level.num_orders = std::string(num_orders);
        }
    }

    return level;
}

std::vector<ExchangeTypes::Level> DataNormalization::ParseLevels(const simdjson::dom::array &levels_array) {
    std::vector<ExchangeTypes::Level> levels;

    for (simdjson::dom::element level_elem : levels_array) {
        if (level_elem.type() == simdjson::dom::element_type::ARRAY) {
            levels.push_back(ParseLevel(level_elem));
        }
    }

    return levels;
}

ExchangeTypes::BookSnapshotData DataNormalization::ParseBookSnapshotData(const simdjson::dom::object &data_obj) {
    ExchangeTypes::BookSnapshotData data;

    // Parse asks
    auto asks = data_obj["asks"];
    if (asks.error() == simdjson::SUCCESS && asks.type() == simdjson::dom::element_type::ARRAY) {
        data.asks = ParseLevels(asks);
    }

    // Parse bids
    auto bids = data_obj["bids"];
    if (bids.error() == simdjson::SUCCESS && bids.type() == simdjson::dom::element_type::ARRAY) {
        data.bids = ParseLevels(bids);
    }

    // Parse timestamps and sequence numbers
    uint64_t tt, t, u;
    if (data_obj["tt"].get_uint64().get(tt) == simdjson::SUCCESS) {
        data.tt = tt;
    }

    if (data_obj["t"].get_uint64().get(t) == simdjson::SUCCESS) {
        data.t = t;
    }

    if (data_obj["u"].get_uint64().get(u) == simdjson::SUCCESS) {
        data.u = u;
    }

    return data;
}

ExchangeTypes::BookDeltaData DataNormalization::ParseBookDeltaData(const simdjson::dom::object &data_obj) {
    ExchangeTypes::BookDeltaData data;

    // Parse update data
    auto update = data_obj["update"];
    if (update.error() == simdjson::SUCCESS && update.type() == simdjson::dom::element_type::OBJECT) {
        simdjson::dom::object update_obj = update;

        // Parse asks in update
        auto asks = update_obj["asks"];
        if (asks.error() == simdjson::SUCCESS && asks.type() == simdjson::dom::element_type::ARRAY) {
            data.update.asks = ParseLevels(asks);
        }

        // Parse bids in update
        auto bids = update_obj["bids"];
        if (bids.error() == simdjson::SUCCESS && bids.type() == simdjson::dom::element_type::ARRAY) {
            data.update.bids = ParseLevels(bids);
        }
    }

    // Parse timestamps and sequence numbers
    uint64_t tt, t, u, pu;
    if (data_obj["tt"].get_uint64().get(tt) == simdjson::SUCCESS) {
        data.tt = tt;
    }

    if (data_obj["t"].get_uint64().get(t) == simdjson::SUCCESS) {
        data.t = t;
    }

    if (data_obj["u"].get_uint64().get(u) == simdjson::SUCCESS) {
        data.u = u;
    }

    if (data_obj["pu"].get_uint64().get(pu) == simdjson::SUCCESS) {
        data.pu = pu;
    }

    return data;
}

ExchangeTypes::MessageBuffer DataNormalization::ParseBookSnapshotMessage(const simdjson::dom::object &root) {
    ExchangeTypes::BookSnapshotResponse response{};

    // Parse base response fields
    int64_t id;
    if (root["id"].get_int64().get(id) == simdjson::SUCCESS) {
        response.id = id;
    }

    std::string_view method;
    if (root["method"].get_string().get(method) == simdjson::SUCCESS) {
        response.method = std::string(method);
    }

    int64_t code;
    if (root["code"].get_int64().get(code) == simdjson::SUCCESS) {
        response.code = static_cast<int>(code);
    }

    // Parse result
    auto result_obj = root["result"];
    if (result_obj.error() == simdjson::SUCCESS) {
        simdjson::dom::object result = result_obj;

        std::string_view instrument_name;
        if (result["instrument_name"].get_string().get(instrument_name) == simdjson::SUCCESS) {
            response.result.instrument_name = std::string(instrument_name);
        }

        std::string_view subscription;
        if (result["subscription"].get_string().get(subscription) == simdjson::SUCCESS) {
            response.result.subscription = std::string(subscription);
        }

        std::string_view channel;
        if (result["channel"].get_string().get(channel) == simdjson::SUCCESS) {
            response.result.channel = std::string(channel);
        }

        int64_t depth;
        if (result["depth"].get_int64().get(depth) == simdjson::SUCCESS) {
            response.result.depth = static_cast<int>(depth);
        }

        // Parse data array
        auto data_array = result["data"];
        if (data_array.error() == simdjson::SUCCESS) {
            for (simdjson::dom::element data_item : data_array) {
                if (data_item.type() == simdjson::dom::element_type::OBJECT) {
                    response.result.data.push_back(ParseBookSnapshotData(data_item));
                }
            }
        }
    }

    return SerializeMessage(response, ExchangeTypes::MessageType::BOOK_SNAPSHOT);
}

ExchangeTypes::MessageBuffer DataNormalization::ParseBookDeltaMessage(const simdjson::dom::object &root) {
    ExchangeTypes::BookDeltaResponse response{};

    // Parse base response fields
    int64_t id;
    if (root["id"].get_int64().get(id) == simdjson::SUCCESS) {
        response.id = id;
    }

    std::string_view method;
    if (root["method"].get_string().get(method) == simdjson::SUCCESS) {
        response.method = std::string(method);
    }

    int64_t code;
    if (root["code"].get_int64().get(code) == simdjson::SUCCESS) {
        response.code = static_cast<int>(code);
    }

    // Parse result
    auto result_obj = root["result"];
    if (result_obj.error() == simdjson::SUCCESS) {
        simdjson::dom::object result = result_obj;

        std::string_view instrument_name;
        if (result["instrument_name"].get_string().get(instrument_name) == simdjson::SUCCESS) {
            response.result.instrument_name = std::string(instrument_name);
        }

        std::string_view subscription;
        if (result["subscription"].get_string().get(subscription) == simdjson::SUCCESS) {
            response.result.subscription = std::string(subscription);
        }

        std::string_view channel;
        if (result["channel"].get_string().get(channel) == simdjson::SUCCESS) {
            response.result.channel = std::string(channel);
        }

        int64_t depth;
        if (result["depth"].get_int64().get(depth) == simdjson::SUCCESS) {
            response.result.depth = static_cast<int>(depth);
        }

        // Parse data array
        auto data_array = result["data"];
        if (data_array.error() == simdjson::SUCCESS) {
            for (simdjson::dom::element data_item : data_array) {
                if (data_item.type() == simdjson::dom::element_type::OBJECT) {
                    response.result.data.push_back(ParseBookDeltaData(data_item));
                }
            }
        }
    }

    return SerializeMessage(response, ExchangeTypes::MessageType::BOOK_DELTA_UPDATE);
}

ExchangeTypes::MessageBuffer DataNormalization::ParseTradeMessage(const simdjson::dom::object &root) {
    ExchangeTypes::TradeResponse response{};

    // Parse base response fields
    int64_t id;
    if (root["id"].get_int64().get(id) == simdjson::SUCCESS) {
        response.id = id;
    }

    std::string_view method;
    if (root["method"].get_string().get(method) == simdjson::SUCCESS) {
        response.method = std::string(method);
    }

    int64_t code;
    if (root["code"].get_int64().get(code) == simdjson::SUCCESS) {
        response.code = static_cast<int>(code);
    }

    // Parse result
    auto result_obj = root["result"];
    if (result_obj.error() == simdjson::SUCCESS) {
        simdjson::dom::object result = result_obj;

        std::string_view instrument_name;
        if (result["instrument_name"].get_string().get(instrument_name) == simdjson::SUCCESS) {
            response.result.instrument_name = std::string(instrument_name);
        }

        std::string_view subscription;
        if (result["subscription"].get_string().get(subscription) == simdjson::SUCCESS) {
            response.result.subscription = std::string(subscription);
        }

        std::string_view channel;
        if (result["channel"].get_string().get(channel) == simdjson::SUCCESS) {
            response.result.channel = std::string(channel);
        }

        // Parse data array
        auto data_array = result["data"];
        if (data_array.error() == simdjson::SUCCESS) {
            for (simdjson::dom::element data_item : data_array) {
                if (data_item.type() == simdjson::dom::element_type::OBJECT) {
                    response.result.data.push_back(ParseTradeData(data_item));
                }
            }
        }
    }

    return SerializeMessage(response, ExchangeTypes::MessageType::TRADE);
}

ExchangeTypes::TradeData DataNormalization::ParseTradeData(const simdjson::dom::object &data_obj) {
    ExchangeTypes::TradeData data;

    // Parse trade ID (d)
    std::string_view d;
    if (data_obj["d"].get_string().get(d) == simdjson::SUCCESS) {
        data.d = std::string(d);
    }

    // Parse trade timestamp in milliseconds (t)
    uint64_t t;
    if (data_obj["t"].get_uint64().get(t) == simdjson::SUCCESS) {
        data.t = t;
    }

    // Parse trade quantity (q)
    std::string_view q;
    if (data_obj["q"].get_string().get(q) == simdjson::SUCCESS) {
        data.q = std::string(q);
    }

    // Parse trade price (p)
    std::string_view p;
    if (data_obj["p"].get_string().get(p) == simdjson::SUCCESS) {
        data.p = std::string(p);
    }

    // Parse side (s) - BUY or SELL
    std::string_view s;
    if (data_obj["s"].get_string().get(s) == simdjson::SUCCESS) {
        data.s = std::string(s);
    }

    // Parse instrument name (i)
    std::string_view i;
    if (data_obj["i"].get_string().get(i) == simdjson::SUCCESS) {
        data.i = std::string(i);
    }

    return data;
}
