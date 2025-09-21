#include "ExchangeConnectivity.h"

#include <iostream>

ExchangeConnectivity::ExchangeConnectivity()
    : ctx_(ssl::context::tlsv12_client), ws_(ioc_, ctx_) {}

void ExchangeConnectivity::ConnectAndListen() {
    try {
        const std::string host = "stream.crypto.com";
        const std::string target = "/exchange/v1/market";

        if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(),
                                      host.c_str())) {
            throw beast::system_error(static_cast<int>(::ERR_get_error()),
                                      net::error::get_ssl_category());
        }

        tcp::resolver resolver{ioc_};
        auto const results = resolver.resolve(host, "443");
        net::connect(ws_.next_layer().next_layer(), results);

        ws_.next_layer().handshake(ssl::stream_base::client);
        ws_.handshake(host, target);

        std::cout << "Connected to Crypto.com WebSocket feed." << std::endl;

        beast::flat_buffer buffer;
        while (true) {
            ws_.read(buffer);
            std::string message(static_cast<const char *>(buffer.data().data()),
                                buffer.data().size());
            std::cout << "Received: " << message << std::endl;
            buffer.consume(buffer.size());
        }
    } catch (const std::exception &e) {
        std::cerr << "WebSocket error: " << e.what() << std::endl;
    }
}
