#include "../include/orderbook.hpp"

#include <sys/socket.h>
#include <libkern/OSByteOrder.h>

template <typename T>
void Orderbook::rebalance(T& orders_list) {
    
    int size_difference = orders_list.size() - MAX_LEVELS;
    if (size_difference > 0) {
        orders_list.erase(orders_list.begin(), orders_list.begin() + size_difference);
    }
}

template <typename T, typename Compare>
void Orderbook::update_level(const std::string& exchange_id, Price price, Volume new_volume,
                    T& orders_list, ExchangeOrderMap& exchanges, Compare comp) {
                

    if (new_volume > 0) {
        exchanges[exchange_id][price] = new_volume;
    }
    else {
        delete_level(exchange_id, price, orders_list, exchanges, comp);
        return;
    }

    double total_volume = get_total_volume_at_price(price, exchanges);
    auto it = std::lower_bound(orders_list.begin(), orders_list.end(), price,
        comp);

    if (it != orders_list.end() && it->first == price) {
        it->second = total_volume;
    }
    else {
        orders_list.insert(it, {price, total_volume});
    }

    rebalance(orders_list);
}

template <typename T, typename Compare>
void Orderbook::delete_level(const std::string& exchange_id, Price price,
                    T& orders_list, ExchangeOrderMap& exchanges, Compare comp) {
    
    
    if (exchanges.count(exchange_id) != 0) {
        exchanges[exchange_id].erase(price);
    }

    double total_volume = get_total_volume_at_price(price, exchanges);
    
    auto it = std::lower_bound(orders_list.begin(), orders_list.end(), price, comp);

    if (it == orders_list.end()) {
        return;
    }

    if (total_volume > 0) {
        it->second = total_volume;
    }
    else {
        orders_list.erase(it);
    }
}

double Orderbook::get_total_volume_at_price(Price price, const ExchangeOrderMap& exchanges) const {
    double total_volume = 0;
    for (const auto& [exchange_id, price_map] : exchanges) {
        auto it = price_map.find(price);
        if (it != price_map.end()) {
            total_volume += it->second;
        }
    }
    return total_volume;
}

void Orderbook::send_snapshot(int client_socket, int n_levels) {
    if (bids_.size() == 0 || asks_.size() == 0) {
        return; 
    }
    
    char buffer[40];

    uint32_t currency_id = currency_id_;
    uint32_t remainingSize = 32;

    auto [bestBidPrice, bestBidVolume] = bids_.back();
    auto [bestAskPrice, bestAskVolume] = asks_.back();

    currency_id = OSSwapHostToBigInt32(currency_id);
    std::memcpy(buffer, &currency_id, 4);

    remainingSize = OSSwapHostToBigInt32(remainingSize);
    std::memcpy(buffer + 4, &remainingSize, 4);

    ToNetworkOrder(bestBidPrice, buffer + 8);
    ToNetworkOrder(bestBidVolume, buffer + 16);
    ToNetworkOrder(bestAskPrice, buffer + 24);
    ToNetworkOrder(bestAskVolume, buffer + 32);

    send(client_socket, buffer, sizeof(buffer), 0);
}

void Orderbook::ToNetworkOrder(double value, char* buffer) {
    uint64_t raw;
    std::memcpy(&raw, &value, sizeof(raw));
    raw = OSSwapHostToBigInt64(raw);
    std::memcpy(buffer, &raw, sizeof(raw));
}

Orderbook::Orderbook(int currency_id) : currency_id_(currency_id) {}


void Orderbook::update_bid(const std::string &exchange_id, Price price, Volume new_volume) {
    update_level(exchange_id, price, new_volume, bids_, exchange_bids_,
        [](const std::pair<Price, Volume>& a, const Price& b) { return a.first < b; });
    //print_bbo();
    //send_snapshot();
}

void Orderbook::update_ask(const std::string &exchange_id, Price price, Volume new_volume) {
    update_level(exchange_id, price, new_volume, asks_, exchange_asks_,
        [](const std::pair<Price, Volume>& a, const Price& b) { return a.first > b; });
    //print_bbo();
    //send_snapshot();
}

void Orderbook::print_bbo() {

    if (bids_.size() > 0) {
        std::cout << "Bid: " << std::fixed << std::setprecision(4) << bids_.begin()->first << " ";
    }
    if (asks_.size() > 0) {
        std::cout << "Ask: " << std::fixed << std::setprecision(4) << asks_.begin()->first;
    }

    std::cout << "\n";

}

size_t Orderbook::get_max_levels() const {
    return MAX_LEVELS;
}

double Orderbook::get_exchange_bid_volume(const std::string& exchange_id, Price price) const {
    auto user_it = exchange_bids_.find(exchange_id);
    if (user_it != exchange_bids_.end()) {
        auto price_it = user_it->second.find(price);
        if (price_it != user_it->second.end()) {
            return price_it->second;
        }
    }

    return 0.0;
}

double Orderbook::get_exchange_ask_volume(const std::string& exchange_id, Price price) const {
    auto price_it = exchange_asks_.find(exchange_id);
    if (price_it != exchange_asks_.end()) {
        auto user_it = price_it->second.find(price);
        if (user_it != price_it->second.end()) {
            return user_it->second;
        }
    }
    return 0.0;
}

void Orderbook::initialize_exchange(const std::string& exchange_id) {
    exchange_bids_[exchange_id];
    exchange_asks_[exchange_id];
}