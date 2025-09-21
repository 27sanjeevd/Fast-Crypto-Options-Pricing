#include "ExchangeConnectivity.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>

// Static member definitions
const std::string ExchangeWebsocketClient::HOST = "stream.crypto.com";
const std::string ExchangeWebsocketClient::PORT = "443";
const std::string ExchangeWebsocketClient::TARGET = "/exchange/v1/market";

ExchangeWebsocketClient::ExchangeWebsocketClient(const std::vector<std::string> &tickers) : running_(false), tickers_(tickers) {
    ctx_.set_verify_mode(ssl::verify_peer);
    ctx_.set_default_verify_paths();
    ws_ = std::make_unique<websocket::stream<beast::ssl_stream<beast::tcp_stream>>>(ioc_, ctx_);
}

ExchangeWebsocketClient::~ExchangeWebsocketClient() { Stop(); }

void ExchangeWebsocketClient::Start() {
    if (running_.load())
        return;

    running_.store(true);

    if (!SSL_set_tlsext_host_name(ws_->next_layer().native_handle(), HOST.c_str())) {
        throw std::runtime_error("Failed to set TLS host name");
    }

    resolver_.async_resolve(HOST, PORT, beast::bind_front_handler(&ExchangeWebsocketClient::OnResolve, this));

    std::thread([this]() {
        try {
            ioc_.run();
        } catch (const std::exception &e) {
            std::cerr << "IO service error: " << e.what() << std::endl;
            running_.store(false);
        }
    }).detach();
}

void ExchangeWebsocketClient::Stop() {
    if (!running_.load())
        return;

    running_.store(false);
    if (ws_) {
        beast::error_code ec;
        ws_->close(websocket::close_code::normal, ec);
    }
    ioc_.stop();
}

void ExchangeWebsocketClient::OnResolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec) {
        std::cerr << "DNS resolution error: " << ec.message() << std::endl;
        return;
    }

    beast::get_lowest_layer(*ws_).expires_after(CONNECTION_TIMEOUT);
    beast::get_lowest_layer(*ws_).async_connect(results, beast::bind_front_handler(&ExchangeWebsocketClient::OnConnect, this));
}

void ExchangeWebsocketClient::OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
    if (ec) {
        std::cerr << "TCP connection error: " << ec.message() << std::endl;
        return;
    }

    beast::get_lowest_layer(*ws_).expires_after(CONNECTION_TIMEOUT);
    ws_->next_layer().async_handshake(ssl::stream_base::client, beast::bind_front_handler(&ExchangeWebsocketClient::OnSslHandshake, this));
}

void ExchangeWebsocketClient::OnSslHandshake(beast::error_code ec) {
    if (ec) {
        std::cerr << "SSL handshake error: " << ec.message() << std::endl;
        return;
    }

    beast::get_lowest_layer(*ws_).expires_never();
    ws_->set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
    ws_->async_handshake(HOST, TARGET, beast::bind_front_handler(&ExchangeWebsocketClient::OnHandshake, this));
}

void ExchangeWebsocketClient::OnHandshake(beast::error_code ec) {
    if (ec) {
        std::cerr << "WebSocket handshake error: " << ec.message() << std::endl;
        return;
    }

    std::cout << "WebSocket connected successfully!" << std::endl;

    // Build subscription for all tickers
    std::ostringstream channels;
    channels << "[";

    bool first = true;
    for (const auto &ticker : tickers_) {
        if (!first) {
            channels << ",";
        }
        // Subscribe to both book (10 deep) and ticker channels
        channels << "\"book." << ticker << ".10\",\"ticker." << ticker << "\"";
        first = false;
    }

    channels << "]";

    std::string subscription =
        "{\"id\":1,\"method\":\"subscribe\",\"params\":{\"channels\":" + channels.str() + ",\"book_subscription_type\":\"SNAPSHOT_AND_UPDATE\"}}";

    std::cout << "Sending subscription: " << subscription << std::endl;

    ws_->async_write(net::buffer(subscription), [this](beast::error_code ec, std::size_t) {
        if (!ec) {
            std::cout << "Subscription sent!" << std::endl;
            ws_->async_read(buffer_, beast::bind_front_handler(&ExchangeWebsocketClient::OnRead, this));
        }
    });
}

void ExchangeWebsocketClient::OnRead(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) {
        if (ec != websocket::error::closed) {
            std::cerr << "Read error: " << ec.message() << std::endl;
        }
        return;
    }

    std::string msg = beast::buffers_to_string(buffer_.data());
    std::cout << "Received: " << msg << std::endl;

    buffer_.consume(buffer_.size());

    if (running_.load()) {
        ws_->async_read(buffer_, beast::bind_front_handler(&ExchangeWebsocketClient::OnRead, this));
    }
}
