# Orderbook Module

This module provides high-performance orderbook management for cryptocurrency trading. It maintains real-time bid/ask levels and provides fast access to best bid/offer (BBO) information crucial for algorithmic trading and options pricing.

## Components

### Orderbook
- **Header**: `Orderbook.h`
- **Implementation**: `Orderbook.cpp`
- **Purpose**: Core orderbook data structure with efficient price level management

### Data
- **Header**: `Data.h`
- **Purpose**: Fundamental data types used throughout the orderbook system

### OrderbookTypes
- **Header**: `OrderbookTypes.h` 
- **Purpose**: Additional type definitions specific to orderbook operations

## Features

- **Real-time Updates**: Handles high-frequency bid/ask updates
- **Efficient Storage**: Optimized data structures for fast access and updates
- **BBO Calculation**: Fast best bid/offer price extraction
- **Price Level Management**: Add, update, and remove price levels efficiently
- **Memory Efficient**: Minimal memory footprint with smart data organization
- **Type Safety**: Strong typing for prices and volumes

## Core Data Types

### Basic Types (Data.h)
```cpp
using Price = double;   // Price representation
using Volume = double;  // Volume/quantity representation

struct BookUpdate {
    Price price;
    Volume volume;
};

struct ReceivedData {
    uint32_t message_type;
    uint32_t currency_name; 
    uint32_t num_levels;
};
```

## API Overview

### Orderbook Class
```cpp
class Orderbook {
public:
    // Update methods
    void UpdateBid(Price price, Volume new_volume);
    void UpdateAsk(Price price, Volume new_volume);
    
    // BBO access
    void PrintBbo();
    std::pair<Price, Price> ReturnBbo();
};
```

## Usage Examples

### Basic Orderbook Operations
```cpp
#include "Orderbook/Orderbook.h"

int main() {
    Orderbook book;
    
    // Add initial bid levels
    book.UpdateBid(99.50, 100.0);  // Price: $99.50, Volume: 100
    book.UpdateBid(99.25, 250.0);  // Price: $99.25, Volume: 250
    book.UpdateBid(99.00, 500.0);  // Price: $99.00, Volume: 500
    
    // Add initial ask levels  
    book.UpdateAsk(100.00, 150.0); // Price: $100.00, Volume: 150
    book.UpdateAsk(100.25, 200.0); // Price: $100.25, Volume: 200
    book.UpdateAsk(100.50, 300.0); // Price: $100.50, Volume: 300
    
    // Display current best bid/offer
    book.PrintBbo();
    
    // Get BBO programmatically
    auto [best_bid, best_ask] = book.ReturnBbo();
    std::cout << "Best Bid: " << best_bid << ", Best Ask: " << best_ask << std::endl;
    
    // Update existing level
    book.UpdateBid(99.50, 75.0);   // Reduce volume at $99.50
    
    // Remove level (set volume to 0)
    book.UpdateBid(99.25, 0.0);    // Remove $99.25 bid level
    
    return 0;
}
```

### Integration with Feed Processing
```cpp
#include "Orderbook/Orderbook.h"
#include "FeedProcessing/FeedProcessing.h"

class TradingSystem {
private:
    std::unordered_map<std::string, std::unique_ptr<Orderbook>> books_;
    
public:
    void ProcessBookUpdate(const std::string& symbol, 
                          const std::vector<BookUpdate>& bid_updates,
                          const std::vector<BookUpdate>& ask_updates) {
        
        auto& book = GetOrCreateOrderbook(symbol);
        
        // Apply bid updates
        for (const auto& update : bid_updates) {
            book->UpdateBid(update.price, update.volume);
        }
        
        // Apply ask updates  
        for (const auto& update : ask_updates) {
            book->UpdateAsk(update.price, update.volume);
        }
        
        // Get updated BBO for pricing calculations
        auto [best_bid, best_ask] = book->ReturnBbo();
        NotifyPricingEngine(symbol, best_bid, best_ask);
    }
    
private:
    Orderbook* GetOrCreateOrderbook(const std::string& symbol) {
        if (books_.find(symbol) == books_.end()) {
            books_[symbol] = std::make_unique<Orderbook>();
        }
        return books_[symbol].get();
    }
};
```

## Data Structure Design

### Price Level Storage
- **Bids**: Stored in descending price order (highest first)
- **Asks**: Stored in ascending price order (lowest first)  
- **Efficient Access**: Vector-based storage for cache-friendly access
- **BBO Optimization**: Best prices always at front of containers

### Update Strategy
- **Insert**: New price levels added in correct sorted position
- **Update**: Existing levels modified in-place
- **Delete**: Zero-volume updates remove price levels
- **Cleanup**: Automatic removal of empty price levels

## Performance Characteristics

### Time Complexity
- **BBO Access**: O(1) - Best prices at container front
- **Update Existing Level**: O(log n) - Binary search for price level
- **Insert New Level**: O(n) - Maintain sorted order
- **Delete Level**: O(n) - Remove and shift elements

### Space Complexity
- **Memory Usage**: O(n) where n is number of active price levels
- **Typical Books**: 10-50 levels per side for liquid instruments
- **Deep Books**: Can handle hundreds of levels efficiently

## Build Target

Build the orderbook module:
```bash
bazel build //Orderbook:Orderbook
```

Run tests:
```bash
bazel test //Orderbook:OrderbookTest
```

## Testing

The module includes comprehensive tests covering:
- Basic bid/ask updates
- BBO calculation accuracy
- Price level management
- Edge cases (empty books, single levels)
- Performance benchmarks

## Integration Points

### Input Sources
- **FeedProcessing**: Normalized market data updates
- **ExchangeConnectivity**: Direct exchange feed processing

### Output Consumers
- **Pricing**: Options pricing algorithms requiring BBO data
- **Risk Management**: Position and exposure calculations
- **Analytics**: Market microstructure analysis

## Thread Safety

- **Single-threaded**: Designed for single-threaded access per instrument
- **Multi-instrument**: Safe to use different orderbook instances across threads
- **No Locks**: Lock-free design for maximum performance in single-threaded context

## Future Enhancements

- FPGA-optimized data structures
- Compressed price level storage
- Historical depth snapshots
- Advanced order book analytics
