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
    using ExchangeOrderMap = std::unordered_map<std::string, std::unordered_map<double, double>>;

    using Price = double;
    using Volume = double;

    static constexpr size_t MAX_LEVELS = 10000;
    static constexpr size_t TOP_LEVELS = 5;

    std::vector<std::pair<Price, Volume>> bids_;
    std::vector<std::pair<Price, Volume>> asks_;

    ExchangeOrderMap exchange_bids_;
    ExchangeOrderMap exchange_asks_;

    uint32_t currency_id_;

    template <typename T>
    void rebalance(T& orders_list);

    template <typename T, typename Compare>
    void update_level(const std::string& exchange_id, Price price, Volume new_volume,
                    T& orders_list, ExchangeOrderMap& exchanges, Compare comp);

    template <typename T, typename Compare>
    void delete_level(const std::string& exchange_id, Price price,
                    T& orders_list, ExchangeOrderMap& exchanges, Compare comp);

    double get_total_volume_at_price(Price price, 
                                const ExchangeOrderMap& exchanges) const;

    void ToNetworkOrder(double value, char* buffer);

    template <typename T, typename Compare>
    bool IsInFirstNKeys(T& orders_list, Price price, Compare comp);

public:
    Orderbook(int currency_id);

    void update_bid(const std::string &exchange_id, Price price, Volume new_volume);
    void update_ask(const std::string &exchange_id, Price price, Volume new_volume);

    void print_bbo(); 
    size_t get_max_levels() const;

    double get_exchange_bid_volume(const std::string& exchange_id, Price price) const;
    double get_exchange_ask_volume(const std::string& exchange_id, Price price) const;

    void initialize_exchange(const std::string& exchange_id);
    void send_snapshot(int client_socket, int n_levels);
};

#endif  // ORDERBOOK_HPP