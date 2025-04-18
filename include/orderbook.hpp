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
public:
    using Price = double;
    using Volume = double;
    
    Orderbook(int currency_id);

    void update_bid(const std::string &exchange_id, Price price, Volume new_volume);
    void update_ask(const std::string &exchange_id, Price price, Volume new_volume);

    void print_bbo();
    
    void initialize_exchange(const std::string& exchange_id);
    void send_snapshot(int client_socket, int n_levels);

private:
    std::vector<std::pair<Price, Volume>> bids_;
    std::vector<std::pair<Price, Volume>> asks_;

    uint32_t currency_id_;

    template <typename T>
    void update_level(const std::string& exchange_id, Price price, Volume new_volume,
                    T& orders_list);

    template <typename T>
    void delete_level(const std::string& exchange_id, Price price, T& orders_list);

    void ToNetworkOrder(double value, char* buffer);
};

#endif  // ORDERBOOK_HPP