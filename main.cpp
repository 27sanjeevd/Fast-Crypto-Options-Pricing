#include "ExchangeConnectivity/ExchangeConnectivity.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

int main() {
    ExchangeWebsocketClient client;

    std::cout << "Starting Exchange Websocket Client..." << std::endl;

    client.start();

    std::cout << "Connected! Waiting for messages for 30 seconds..." << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(30));

    std::cout << "Stopping client..." << std::endl;
    client.stop();

    std::cout << "Client stopped." << std::endl;
    return 0;
}
