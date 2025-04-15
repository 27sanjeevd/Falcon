#pragma once

#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <iostream>
#include <iomanip>

#include "data.hpp"

class Orderbook {
private:
    using Price = double;
    using Volume = double;

    std::vector<std::pair<Price, Volume>> bids_;
    std::vector<std::pair<Price, Volume>> asks_;

    uint32_t currency_id_;

    template <typename T, typename Compare>
    void update_level(const std::string& exchange_id, Price price, Volume new_volume,
                    T& orders_list, Compare comp);

    template <typename T, typename Compare>
    void delete_level(const std::string& exchange_id, Price price,
                    T& orders_list, Compare comp);

    void ToNetworkOrder(double value, char* buffer);

    template <typename T, typename Compare>
    bool IsInFirstNKeys(T& orders_list, Price price, Compare comp);

public:
    Orderbook(int currency_id);

    
    void update_bid(const std::string &exchange_id, Price price, Volume new_volume) {
        auto comp = [](const std::pair<Price, Volume>& a, const Price& b) { return a.first < b; };

        if (new_volume == 0) {
            delete_level(exchange_id, price, bids_, comp);
        }
        else {
            update_level(exchange_id, price, new_volume, bids_, comp);
        }
    }

    void update_ask(const std::string &exchange_id, Price price, Volume new_volume) {
        auto comp = [](const std::pair<Price, Volume>& a, const Price& b) { return a.first > b; };

        if (new_volume == 0) {
            delete_level(exchange_id, price, asks_, comp);
        }
        else {
            update_level(exchange_id, price, new_volume, asks_, comp);
        }
    }

    void print_bbo();
    
    void initialize_exchange(const std::string& exchange_id);
    void send_snapshot(int client_socket, int n_levels);
};

template <typename T, typename Compare>
void Orderbook::update_level(const std::string& exchange_id, Price price, Volume new_volume,
                    T& orders_list, Compare comp) {
                

    auto it = std::lower_bound(orders_list.begin(), orders_list.end(), price, comp);

    if (it != orders_list.end() && it->first == price) {
        it->second = new_volume;
    }
    else {
        orders_list.insert(it, {price, new_volume});
    }
}

template <typename T, typename Compare>
void Orderbook::delete_level(const std::string& exchange_id, Price price,
                    T& orders_list, Compare comp) {
    
    
    auto it = std::lower_bound(orders_list.begin(), orders_list.end(), price, comp);
    if (it == orders_list.end()) {
        return;
    }

    orders_list.erase(it);
}

#endif  // ORDERBOOK_HPP