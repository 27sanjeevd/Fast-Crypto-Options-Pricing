#include "Orderbook.h"

#include <libkern/OSByteOrder.h>
#include <sys/socket.h>

template <typename T>
void Orderbook::update_level(Price price, Volume new_volume, T &orders_list) {

    auto it =
        std::lower_bound(orders_list.begin(), orders_list.end(), price,
                         [](const auto &a, double b) { return a.first > b; });

    if (it != orders_list.end() && it->first == price) {
        it->second = new_volume;
    } else {
        orders_list.insert(it, {price, new_volume});
    }
}

template <typename T>
void Orderbook::delete_level(Price price, T &orders_list) {

    auto it =
        std::lower_bound(orders_list.begin(), orders_list.end(), price,
                         [](const auto &a, double b) { return a.first > b; });

    if (it == orders_list.end()) {
        return;
    }

    orders_list.erase(it);
}

void Orderbook::update_bid(Price price, Volume new_volume) {
    if (new_volume == 0) {
        delete_level(price, bids_);
    } else {
        update_level(price, new_volume, bids_);
    }
}

void Orderbook::update_ask(Price price, Volume new_volume) {

    if (new_volume == 0) {
        delete_level(-1 * price, asks_);
    } else {
        update_level(-1 * price, new_volume, asks_);
    }
}

void Orderbook::print_bbo() {

    if (bids_.size() > 0) {
        std::cout << "Bid: " << std::fixed << std::setprecision(4)
                  << bids_.begin()->first << " ";
    }
    if (asks_.size() > 0) {
        std::cout << "Ask: " << std::fixed << std::setprecision(4)
                  << (-1 * asks_.begin()->first);
    }

    std::cout << "\n";
}

std::pair<Price, Price> Orderbook::return_bbo() {
    Price best_bid = bids_.empty() ? 0 : bids_.begin()->first;
    Price best_ask = asks_.empty() ? 0 : -1 * asks_.begin()->first;

    return {best_bid, best_ask};
}