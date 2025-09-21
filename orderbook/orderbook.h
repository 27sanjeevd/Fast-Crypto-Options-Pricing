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
    void UpdateBid(Price price, Volume new_volume);
    void UpdateAsk(Price price, Volume new_volume);

    void PrintBbo();
    std::pair<Price, Price> ReturnBbo();

  private:
    std::vector<std::pair<Price, Volume>> bids_;
    std::vector<std::pair<Price, Volume>> asks_;

    template <typename T> void update_level(Price price, Volume new_volume, T &orders_list);

    template <typename T> void delete_level(Price price, T &orders_list);
};