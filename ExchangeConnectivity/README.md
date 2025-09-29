# ExchangeConnectivity Module

This module provides real-time connectivity to crypto exchanges using WebSocket connections. It handles secure connections, subscription management, and real-time data streaming from cryptocurrency exchanges.

## Components

### ExchangeWebsocketClient
- **Header**: `ExchangeConnectivity.h`
- **Implementation**: `ExchangeConnectivity.cpp` 
- **Purpose**: Manages WebSocket connections to crypto.com exchange for real-time market data

## Features

- **Secure WebSocket Connection**: Uses SSL/TLS encryption for secure data transmission
- **Real-time Data Streaming**: Subscribes to orderbook and trade data feeds
- **Robust Connection Management**: Automatic connection handling with timeout management
- **Multi-instrument Support**: Can subscribe to multiple trading pairs simultaneously
- **Channel Flexibility**: Supports both orderbook and trade data channels

## API Overview

### Constructor
```cpp
ExchangeWebsocketClient(const std::vector<std::string> &tickers, 
                       const std::vector<std::string> &outbound_channels)
```
- `tickers`: List of trading pairs to subscribe to (e.g., "BTC_USDT", "ETH_USDT")
- `outbound_channels`: List of IPC channels for forwarding processed data

### Core Methods
```cpp
void Start();  // Establishes connection and begins data streaming
void Stop();   // Gracefully closes connection and stops streaming
```

## Usage Example

```cpp
#include "ExchangeConnectivity/ExchangeConnectivity.h"

int main() {
    // Define trading pairs of interest
    std::vector<std::string> tickers = {"BTC_USDT", "ETH_USDT", "BTC_ETH"};
    
    // Define output channels for processed data
    std::vector<std::string> channels = {"/tmp/orderbook.sock", "/tmp/trades.sock"};
    
    // Create and start the WebSocket client
    ExchangeWebsocketClient client(tickers, channels);
    client.Start();
    
    // Keep the application running
    std::this_thread::sleep_for(std::chrono::hours(24));
    
    // Graceful shutdown
    client.Stop();
    return 0;
}
```

## Configuration

The client connects to:
- **Host**: `stream.crypto.com`
- **Port**: `443` (HTTPS/WSS)
- **Endpoint**: `/exchange/v1/market`

## Data Types Handled

- **Book Snapshots**: Complete orderbook state
- **Book Deltas**: Incremental orderbook updates
- **Trade Data**: Real-time trade execution information

## Connection Management

- **Automatic Reconnection**: Handles temporary connection drops
- **Timeout Management**: 30-second connection timeout
- **SSL Verification**: Validates exchange SSL certificates
- **Graceful Shutdown**: Properly closes connections on stop

## Dependencies

- **Boost.Beast**: WebSocket and HTTP client functionality
- **Boost.Asio**: Asynchronous I/O operations
- **OpenSSL**: SSL/TLS encryption
- **Types Module**: Exchange-specific data structures

## Build Target

Build this module with:
```bash
bazel build //ExchangeConnectivity:ExchangeConnectivity
```

## Integration

This module is designed to work with:
- **FeedProcessing**: Processes raw exchange data into normalized format
- **Types**: Provides exchange-specific data structures
- **IPCConnection**: Forwards processed data to downstream components

## Thread Safety

- Uses atomic operations for thread-safe state management
- Runs I/O operations in separate detached threads
- Non-copyable and non-movable for resource safety
