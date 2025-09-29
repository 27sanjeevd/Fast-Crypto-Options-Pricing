# FeedProcessing Module

This module processes raw exchange data feeds and converts them into normalized, structured formats suitable for trading algorithms and orderbook management. It acts as the data processing pipeline between exchange connectivity and trading logic.

## Components

### FeedProcessing
- **Header**: `FeedProcessing.h`
- **Implementation**: `FeedProcessing.cpp` 
- **Purpose**: Main processing engine that handles incoming exchange messages and maintains orderbooks

### DataNormalization
- **Header**: `DataNormalization.h`
- **Implementation**: `DataNormalization.cpp`
- **Purpose**: Parses and normalizes JSON messages from exchanges into structured C++ objects

## Features

- **Real-time Message Processing**: Handles high-frequency exchange data streams
- **JSON Parsing**: Fast JSON parsing using SimdJSON for optimal performance
- **Data Normalization**: Converts exchange-specific formats to standardized structures
- **Orderbook Management**: Maintains live orderbooks for multiple instruments
- **BBO (Best Bid/Offer) Tracking**: Provides real-time best price information
- **Message Type Detection**: Automatically identifies and routes different message types
- **Sequence Number Validation**: Ensures data integrity and proper ordering

## API Overview

### FeedProcessing Class
```cpp
// Constructor
FeedProcessing(const std::string &data_input_socket_path, 
               const std::string &bbo_output_socket_path = "/tmp/default_bbo_output.sock");

// Main processing loop
void Run(std::optional<size_t> max_messages = std::nullopt);

// Statistics
uint64_t GetBookSnapshotCount() const;
uint64_t GetBookDeltaCount() const; 
uint64_t GetTradeCount() const;
```

### DataNormalization Class
```cpp
// Parse exchange JSON messages
ExchangeTypes::MessageBuffer ParseExchangeMessage(const std::string &json_str);
```

## Usage Example

```cpp
#include "FeedProcessing/FeedProcessing.h"

int main() {
    // Create feed processor
    FeedProcessing processor("/tmp/exchange_feed.sock", "/tmp/bbo_output.sock");
    
    // Start processing (runs indefinitely)
    processor.Run();
    
    // Get processing statistics
    std::cout << "Processed " << processor.GetBookSnapshotCount() << " snapshots\n";
    std::cout << "Processed " << processor.GetBookDeltaCount() << " deltas\n";
    std::cout << "Processed " << processor.GetTradeCount() << " trades\n";
    
    return 0;
}
```

## Message Types Handled

### Book Snapshots
- Complete orderbook state for an instrument
- Contains full bid/ask levels with prices, sizes, and order counts
- Used to initialize or reset orderbook state

### Book Delta Updates
- Incremental changes to orderbook state
- More efficient than full snapshots for frequent updates
- Maintains sequence numbers for integrity checking

### Trade Messages
- Individual trade execution information
- Contains price, quantity, timestamp, and side information
- Used for trade analysis and market activity tracking

## Data Flow

```
Raw JSON Messages → DataNormalization → Structured Objects → FeedProcessing → Orderbooks → BBO Updates
```

1. **Input**: Raw JSON messages from exchange WebSocket
2. **Parsing**: SimdJSON converts JSON to structured data
3. **Processing**: Messages routed by type and processed
4. **Orderbook Updates**: Live orderbooks updated with new data
5. **BBO Generation**: Best bid/offer extracted and forwarded
6. **Output**: Normalized data sent to downstream components

## Performance Optimizations

- **SimdJSON**: Ultra-fast JSON parsing library
- **Memory Reuse**: Efficient buffer management
- **Minimal Copying**: Direct pointer access where possible
- **Sequence Validation**: Fast integrity checking
- **Hash Maps**: O(1) orderbook lookups by instrument

## Configuration

### Input Sources
- Unix domain sockets from ExchangeConnectivity module
- Configurable socket paths for different data streams

### Output Destinations  
- BBO updates via IPC sockets
- Configurable output socket paths

## Build Targets

Build individual components:
```bash
bazel build //FeedProcessing:DataNormalization
bazel build //FeedProcessing:FeedProcessing
```

Run tests:
```bash
bazel test //FeedProcessing:DataNormalizationTest
bazel test //FeedProcessing:FeedProcessingTest
```

## Dependencies

- **SimdJSON**: High-performance JSON parsing
- **IPCConnection**: Inter-process communication
- **Orderbook**: Orderbook management
- **Types**: Exchange data structures

## Integration Points

### Upstream
- **ExchangeConnectivity**: Receives raw exchange data

### Downstream
- **Pricing**: Uses normalized data for option pricing
- **Orderbook**: Maintains live market state
- Any component requiring BBO data via IPC

## Error Handling

- Malformed JSON messages are logged and skipped
- Sequence number gaps trigger re-synchronization
- Network errors are propagated to calling code
- Memory allocation failures are handled gracefully

## Thread Safety

- Single-threaded processing model for predictable performance
- Thread-safe IPC operations
- No shared mutable state between threads
