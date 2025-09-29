# IPC Connection Module

This module provides Inter-Process Communication (IPC) functionality using Unix domain sockets. The module is split into separate components for better modularity and features a clean, unified API for binary data transmission.

## Components

### IPCReceiver
- **Header**: `IPCReceiver.h`
- **Implementation**: `IPCReceiver.cpp` 
- **Purpose**: Server-side component that listens for incoming connections and receives binary data

### IPCSender  
- **Header**: `IPCSender.h`
- **Implementation**: `IPCSender.cpp`
- **Purpose**: Client-side component that connects to servers and sends binary data

## API Design Philosophy

The IPC module follows a **single method approach**:
- **One way to send**: `SendData(const void* data, size_t size)`
- **One way to receive**: `ReadData()` returns `std::vector<uint8_t>`
- **Type agnostic**: Works with strings, vectors, structs, arrays, or any binary data
- **No backward compatibility baggage**: Clean, simple interface

## Usage Examples

### Using IPCReceiver (Server)
```cpp
#include "IPCConnection/IPCReceiver.h"

// Create receiver with a socket name
IPC::IPCReceiver receiver("my_socket");

// Initialize and start listening
if (receiver.Initialize()) {
    // Read binary data (blocking call)
    auto data = receiver.ReadData();
    if (!data.empty()) {
        // Convert to string if needed
        std::string message(data.begin(), data.end());
        std::cout << "Received: " << message << std::endl;
    }
}
```

### Using IPCSender (Client)
```cpp
#include "IPCConnection/IPCSender.h"

// Create sender
IPC::IPCSender sender;

// Connect to receiver's socket
if (sender.Connect("/tmp/my_socket.sock")) {
    // Send a string
    std::string message = "Hello, World!";
    sender.SendData(message.c_str(), message.size());
    
    // Send binary data
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0xFF};
    sender.SendData(data.data(), data.size());
    
    // Send a struct
    MyStruct s;
    sender.SendData(&s, sizeof(s));
    
    // Send an array
    int numbers[5] = {1, 2, 3, 4, 5};
    sender.SendData(numbers, sizeof(numbers));
}
```

### Complete Example (Server & Client)
```cpp
#include "IPCConnection/IPCReceiver.h"
#include "IPCConnection/IPCSender.h"
#include <thread>

void server() {
    IPC::IPCReceiver receiver("example_socket");
    if (receiver.Initialize()) {
        auto data = receiver.ReadData();
        std::string message(data.begin(), data.end());
        std::cout << "Server received: " << message << std::endl;
    }
}

void client() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Let server start
    
    IPC::IPCSender sender;
    if (sender.Connect("/tmp/example_socket.sock")) {
        std::string msg = "Hello from client!";
        sender.SendData(msg.c_str(), msg.size());
    }
}

int main() {
    std::thread server_thread(server);
    std::thread client_thread(client);
    
    server_thread.join();
    client_thread.join();
    return 0;
}
```

## Build Targets

- `//IPCConnection:IPCReceiver` - Just the receiver component
- `//IPCConnection:IPCSender` - Just the sender component

## Testing

Run the tests with:
```bash
bazel test //IPCConnection:IPCConnectionTest
```

The tests demonstrate:
- Basic string transmission
- Binary data transmission
- Multiple message handling
- All using the unified `SendData()` / `ReadData()` API

## Protocol

Data is transmitted using a simple, reliable protocol:
1. **Size Prefix**: 4-byte data length (uint32_t) in host byte order
2. **Data Payload**: The actual binary data

This ensures reliable framing over the stream socket, guaranteeing complete message delivery.

## Architecture Benefits

- **Clean Separation**: Transport layer (IPC framing) vs Application layer (data interpretation)
- **No Duplication**: Size information only exists in the transport layer where it's needed
- **Type Safety**: Binary-first design works with any data type
- **Simplicity**: Single method for sending, single method for receiving
- **Flexibility**: From JSON strings to binary structs - one API handles all

## Migration Notes

The API has been simplified to eliminate redundancy:
- ❌ `SendMessage(string)` - removed
- ❌ `ReadMessage()` - removed  
- ❌ `SendData(vector)` - removed
- ✅ `SendData(void*, size_t)` - universal sender
- ✅ `ReadData()` - universal receiver
