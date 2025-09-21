#include "ExchangeConnectivity/ExchangeConnectivity.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

int main() {
    // Define the tickers to subscribe to
    std::vector<std::string> tickers = {"BTCUSD-PERP", "ETHUSD-PERP"};
    ExchangeWebsocketClient client(tickers);

    std::cout << "Starting Exchange Websocket Client..." << std::endl;

    client.Start();

    std::cout << "Connected! Waiting for messages for 30 seconds..." << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(30));

    std::cout << "Stopping client..." << std::endl;
    client.Stop();

    std::cout << "Client stopped." << std::endl;
    return 0;
}
