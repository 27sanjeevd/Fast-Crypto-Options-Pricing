#pragma once

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <unordered_map>
#include <vector>

#include "data.h"

class Orderbook {
public:
  using Price = double;
  using Volume = double;

  void update_bid(Price price, Volume new_volume);
  void update_ask(Price price, Volume new_volume);

  void print_bbo();

  void initialize_exchange(const std::string &exchange_id);
  void send_snapshot(int client_socket, int n_levels);

private:
  std::vector<std::pair<Price, Volume>> bids_;
  std::vector<std::pair<Price, Volume>> asks_;

  template <typename T>
  void update_level(Price price, Volume new_volume, T &orders_list);

  template <typename T> void delete_level(Price price, T &orders_list);

  void ToNetworkOrder(double value, char *buffer);
};