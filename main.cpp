#include "Orderbook/Orderbook.h"

#include <iostream>
#include <unordered_map>

int main() {
  std::unordered_map<std::string, Orderbook> orderbooks;
  orderbooks["BTC-USD"].update_bid(50000.0, 1.5);
  orderbooks["BTC-USD"].update_ask(51000.0, 2.0);

  orderbooks["BTC-USD"].update_bid(50001.0, 2.0);

  orderbooks["BTC-USD"].print_bbo();

  return 0;
}
