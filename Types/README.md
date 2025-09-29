# Types Module

This module provides fundamental data structures and type definitions used throughout the fast crypto options pricing system. It defines the core messaging protocol, exchange-specific data formats, and shared utilities for type-safe communication between components.

## Components

### SharedTypes
- **Header**: `SharedTypes.h`
- **Implementation**: `SharedTypes.cpp`
- **Purpose**: Core messaging infrastructure and base types used across all modules

### ExchangeBookTypes  
- **Header**: `ExchangeBookTypes.h`
- **Purpose**: Data structures specific to orderbook messages from cryptocurrency exchanges

### TradeTypes
- **Header**: `TradeTypes.h` 
- **Purpose**: Data structures for trade execution messages and related information

## Features

- **Type Safety**: Strong typing for financial data to prevent errors
- **Message Protocol**: Standardized messaging format for IPC communication
- **Exchange Compatibility**: Native support for crypto.com exchange formats
- **Binary Serialization**: Efficient binary message encoding/decoding
- **Template Utilities**: Generic helpers for type casting and message handling
- **Extensible Design**: Easy to add new exchange formats and message types

## Core Type System

### Message Infrastructure (SharedTypes.h)

#### Message Types
```cpp
enum class MessageType : uint32_t {
    BOOK_SNAPSHOT = 1,      // Complete orderbook state
    BOOK_DELTA_UPDATE = 2,  // Incremental orderbook changes  
    TRADE = 3,              // Trade execution data
    UNKNOWN = 0             // Invalid/unrecognized messages
};
```

#### Message Structure
```cpp
struct MessageHeader {
    MessageType type;       // Identifies message content type
};

struct BaseResponse {
    MessageHeader header;   // Must be first field for casting
    int64_t id;            // Message/request identifier
    std::string method;     // Exchange method name
    int code;              // Response code
};
```

#### Binary Message Handling
```cpp
using MessageBuffer = std::vector<uint8_t>;

// Safe type casting for binary messages
template<typename T>
const T* CastToMessage(const MessageBuffer& buffer);

template<typename T> 
T* CastToMessage(MessageBuffer& buffer);
```

#### Subscription Management
```cpp
struct SubscriptionParams {
    std::vector<std::string> channels;          // Data channels to subscribe to
    std::string book_subscription_type;         // Orderbook subscription mode
    int book_update_frequency = 0;              // Update frequency in milliseconds
};

struct SubscriptionRequest {
    int64_t id;                    // Request identifier
    std::string method;            // Subscription method
    SubscriptionParams params;     // Subscription parameters
    
    std::string to_string() const; // JSON serialization
};
```

### Orderbook Types (ExchangeBookTypes.h)

#### Price Level Representation
```cpp
struct Level {
    std::string price;      // Price as string (preserves precision)
    std::string size;       // Volume/quantity as string
    std::string num_orders; // Number of orders at this price level
    
    Level(const std::string& p, const std::string& s, const std::string& n);
    Level() = default;
};
```

#### Orderbook Data Structures
```cpp
struct BookUpdateData {
    std::vector<Level> asks;    // Ask price levels
    std::vector<Level> bids;    // Bid price levels  
};

struct BookSnapshotData {
    std::vector<Level> asks;    // Complete ask side
    std::vector<Level> bids;    // Complete bid side
    uint64_t tt;               // Transaction time
    uint64_t t;                // Timestamp
    uint64_t u;                // Update sequence number
};
```

#### Message Responses
```cpp
struct BookSnapshotResponse : public BaseResponse {
    // Contains complete orderbook state
};

struct BookDeltaResponse : public BaseResponse {  
    // Contains incremental orderbook changes
};
```

### Trade Types (TradeTypes.h)

#### Individual Trade Data
```cpp
struct TradeData {
    std::string d;  // Trade ID (unique identifier)
    uint64_t t;     // Trade timestamp in milliseconds  
    std::string q;  // Trade quantity/volume
    std::string p;  // Trade price
    std::string s;  // Side: "BUY" or "SELL"
    std::string i;  // Instrument name (trading pair)
};
```

#### Trade Message Structure
```cpp
struct TradeResult {
    std::string instrument_name;    // Trading pair
    std::string subscription;       // Subscription identifier  
    std::string channel;           // Data channel name
    std::vector<TradeData> data;   // Array of trade executions
};

struct TradeResponse : public BaseResponse {
    TradeResult result;            // Trade execution details
};
```

## Usage Examples

### Message Processing
```cpp
#include "Types/SharedTypes.h"
#include "Types/ExchangeBookTypes.h" 
#include "Types/TradeTypes.h"

void ProcessMessage(const ExchangeTypes::MessageBuffer& buffer) {
    // Check message type
    auto* header = ExchangeTypes::CastToMessage<ExchangeTypes::MessageHeader>(buffer);
    if (!header) {
        std::cerr << "Invalid message buffer" << std::endl;
        return;
    }
    
    switch (header->type) {
        case ExchangeTypes::MessageType::BOOK_SNAPSHOT: {
            auto* snapshot = ExchangeTypes::CastToMessage<ExchangeTypes::BookSnapshotResponse>(buffer);
            ProcessBookSnapshot(snapshot);
            break;
        }
        
        case ExchangeTypes::MessageType::BOOK_DELTA_UPDATE: {
            auto* delta = ExchangeTypes::CastToMessage<ExchangeTypes::BookDeltaResponse>(buffer);  
            ProcessBookDelta(delta);
            break;
        }
        
        case ExchangeTypes::MessageType::TRADE: {
            auto* trade = ExchangeTypes::CastToMessage<ExchangeTypes::TradeResponse>(buffer);
            ProcessTrade(trade);
            break;
        }
        
        default:
            std::cerr << "Unknown message type" << std::endl;
            break;
    }
}
```

### Subscription Creation
```cpp
#include "Types/SharedTypes.h"

ExchangeTypes::SubscriptionRequest CreateOrderbookSubscription(
    const std::vector<std::string>& instruments) {
    
    ExchangeTypes::SubscriptionRequest request;
    request.id = GetNextRequestId();
    request.method = "subscribe";
    
    // Configure orderbook subscription
    for (const auto& instrument : instruments) {
        request.params.channels.push_back("book." + instrument + ".10");  // 10-level book
    }
    request.params.book_subscription_type = "snapshot_and_updates";
    request.params.book_update_frequency = 100; // 100ms updates
    
    return request;
}

// Send subscription as JSON
std::string json = request.to_string();
websocket.send(json);
```

### Type-Safe Data Access
```cpp
void ProcessBookSnapshot(const ExchangeTypes::BookSnapshotResponse* response) {
    if (!response) return;
    
    std::cout << "Processing snapshot for request ID: " << response->id << std::endl;
    std::cout << "Method: " << response->method << std::endl;
    std::cout << "Response code: " << response->code << std::endl;
    
    // Access book data (implementation depends on specific response structure)
    // Note: Actual data access would depend on the complete response structure
}

void ProcessTrade(const ExchangeTypes::TradeResponse* response) {
    if (!response) return;
    
    const auto& result = response->result;
    std::cout << "Trade data for " << result.instrument_name << ":" << std::endl;
    
    for (const auto& trade : result.data) {
        std::cout << "  Trade ID: " << trade.d << std::endl;
        std::cout << "  Price: " << trade.p << std::endl;
        std::cout << "  Quantity: " << trade.q << std::endl;
        std::cout << "  Side: " << trade.s << std::endl;
        std::cout << "  Timestamp: " << trade.t << std::endl;
    }
}
```

## Design Principles

### String-Based Precision
- Financial data stored as strings to preserve exact precision
- Avoids floating-point rounding errors in price calculations
- Maintains compatibility with exchange native formats

### Binary Message Protocol
- Efficient binary serialization for high-frequency data
- Size-prefixed messages for reliable stream framing
- Type-safe casting with compile-time checks

### Extensibility  
- Easy to add new message types via enum extension
- Template-based utilities work with new types automatically
- Minimal code changes needed for new exchange support

## Build Targets

Build individual type libraries:
```bash
bazel build //Types:SharedTypes
bazel build //Types:ExchangeBookTypes  
bazel build //Types:TradeTypes
```

## Dependencies

- **Standard Library**: STL containers and utilities
- **No External Dependencies**: Self-contained type definitions
- **Header-Only**: Most components are header-only for fast compilation

## Integration Points

### Used By
- **ExchangeConnectivity**: Message creation and parsing
- **FeedProcessing**: Data normalization and routing  
- **Orderbook**: Price level management
- **Pricing**: Financial calculations
- **IPCConnection**: Binary message transmission

### Thread Safety
- **Immutable Data**: Most structures are read-only after creation
- **No Shared State**: Types are designed for pass-by-value or const reference
- **Thread-Safe**: Safe to use across multiple threads without synchronization

## Message Protocol Specification

### Binary Message Format
```
[Header: MessageType (4 bytes)] [Payload: Variable length]
```

### JSON Subscription Format  
```json
{
    "id": 1234567890,
    "method": "subscribe", 
    "params": {
        "channels": ["book.BTC_USDT.10", "trade.BTC_USDT"],
        "book_subscription_type": "snapshot_and_updates",
        "book_update_frequency": 100
    }
}
```

This types module serves as the foundation for all inter-component communication in the fast crypto options pricing system, ensuring type safety, performance, and extensibility.
