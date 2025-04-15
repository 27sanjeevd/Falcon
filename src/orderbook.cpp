#include "../include/orderbook.hpp"

#include <sys/socket.h>
#include <libkern/OSByteOrder.h>

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

void Orderbook::print_bbo() {

    if (bids_.size() > 0) {
        std::cout << "Bid: " << std::fixed << std::setprecision(4) << bids_.begin()->first << " ";
    }
    if (asks_.size() > 0) {
        std::cout << "Ask: " << std::fixed << std::setprecision(4) << asks_.begin()->first;
    }

    std::cout << "\n";

}