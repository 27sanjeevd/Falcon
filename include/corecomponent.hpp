#pragma once

#ifndef CORECOMPONENT_HPP
#define CORECOMPONENT_HPP

#include "data.hpp"
#include "exchange.hpp"
#include "websockets/coinbase_ws.hpp"
#include "websockets/crypto_ws.hpp"

#include <vector>
#include <unordered_map>
#include <memory>

class CoreComponent {
public:
    void Run();

    void ReceiveConnections();

    void ConnectionHandler(int client_socket);

    int ProcessRequest(const char* request, int client_socket);

    void AddWebsocketConnection(uint32_t currency_id);

private:
    int server_fd_ = -1;

    std::unordered_map<uint32_t, std::shared_ptr<Orderbook>> open_orderbooks_;

    void ToNetworkOrder(double value, char* buffer);
};

#endif // CORECOMPONENT_HPP
