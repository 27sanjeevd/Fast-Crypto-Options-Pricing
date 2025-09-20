#include "orderbook/orderbook.h"
#include <iostream>

int main() {
    OrderBook ob;
    ob.add_order(100, 5);
    std::cout << "Order added to orderbook." << std::endl;
    return 0;
}
