#include "../include/orderbook.hpp"

#include <sys/socket.h>
#include <libkern/OSByteOrder.h>


template <typename T>
void Orderbook::update_level(const std::string& exchange_id, Price price, Volume new_volume,
                    T& orders_list) {

    auto it = std::lower_bound(
        orders_list.begin(), 
        orders_list.end(), 
        price,
        [](const auto& a, double b) { return a.first < b; }
    );

    if (it != orders_list.end() && it->first == price) {
        it->second = new_volume;
    }
    else {
        orders_list.insert(it, {price, new_volume});
    }
}

template <typename T>
void Orderbook::delete_level(const std::string& exchange_id, Price price,
                    T& orders_list) {
    
    
    auto it = std::lower_bound(
        orders_list.begin(), 
        orders_list.end(), 
        price,
        [](const auto& a, double b) { return a.first < b; }
    );
    
    if (it == orders_list.end()) {
        return;
    }

    orders_list.erase(it);
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

    bestAskPrice *= -1;

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
    if (new_volume == 0) {
        delete_level(exchange_id, price, bids_);
    }
    else {
        update_level(exchange_id, price, new_volume, bids_);
    }
}

void Orderbook::update_ask(const std::string &exchange_id, Price price, Volume new_volume) {

    if (new_volume == 0) {
        delete_level(exchange_id, -1 * price, asks_);
    }
    else {
        update_level(exchange_id, -1 * price, new_volume, asks_);
    }
}

void Orderbook::print_bbo() {

    if (bids_.size() > 0) {
        std::cout << "Bid: " << std::fixed << std::setprecision(4) << bids_.begin()->first << " ";
    }
    if (asks_.size() > 0) {
        std::cout << "Ask: " << std::fixed << std::setprecision(4) << (-1 * asks_.begin()->first);
    }

    std::cout << "\n";

}