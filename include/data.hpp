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

#endif // DATA_HPP