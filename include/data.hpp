#pragma once

#ifndef DATA_HPP
#define DATA_HPP

#include <vector>
#include <tuple>


struct ReceivedData {
    uint32_t message_type;
    uint32_t currency_name;
    uint32_t num_levels;
};

struct BookUpdate {
    double price;
    double volume;
};

/*
//price, volume, num orders
struct Orderbook_State {
    std::vector<std::tuple<double, double, uint64_t>> bids; 
    std::vector<std::tuple<double, double, uint64_t>> asks; 
};

struct OrderUpdate {
    std::string_view side;
    double price_level;
    double new_quantity;
};
*/

#endif // DATA_HPP