#pragma once

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <unordered_map>
#include <vector>

#include "Data.h"

class Orderbook {
  public:
    void update_bid(Price price, Volume new_volume);
    void update_ask(Price price, Volume new_volume);

    void print_bbo();
    std::pair<Price, Price> return_bbo();

  private:
    std::vector<std::pair<Price, Volume>> bids_;
    std::vector<std::pair<Price, Volume>> asks_;

    template <typename T>
    void update_level(Price price, Volume new_volume, T &orders_list);

    template <typename T> void delete_level(Price price, T &orders_list);
};