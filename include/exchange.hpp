#pragma once

#ifndef EXCHANGE_HPP
#define EXCHANGE_HPP

#include "data.hpp"

#include <string>
#include <optional>

class Exchange {
public:
    virtual ~Exchange() = default;

    virtual std::optional<std::string> ReturnRequest(const std::string &url) = 0;

    virtual std::optional<std::string> get_asset_name_conversion(const std::string &name)  = 0;

    virtual std::string get_name() = 0;
};

#endif // EXCHANGE_HPP