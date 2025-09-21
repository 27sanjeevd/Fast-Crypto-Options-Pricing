#pragma once

#include <atomic>
#include <memory>
#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = net::ip::tcp;

/**
 * @brief Simplified WebSocket client for crypto.com exchange data
 */
class ExchangeWebsocketClient {
  public:
    static constexpr auto CONNECTION_TIMEOUT = std::chrono::seconds(30);

    ExchangeWebsocketClient();
    ~ExchangeWebsocketClient();

    // Non-copyable and non-movable
    ExchangeWebsocketClient(const ExchangeWebsocketClient &) = delete;
    ExchangeWebsocketClient &operator=(const ExchangeWebsocketClient &) = delete;
    ExchangeWebsocketClient(ExchangeWebsocketClient &&) = delete;
    ExchangeWebsocketClient &operator=(ExchangeWebsocketClient &&) = delete;

    void start();
    void stop();

  private:
    static const std::string HOST;
    static const std::string PORT;
    static const std::string TARGET;

    // Async callback handlers
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
    void on_ssl_handshake(beast::error_code ec);
    void on_handshake(beast::error_code ec);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);

    // Network components
    net::io_context ioc_;
    ssl::context ctx_{ssl::context::tlsv12_client};
    tcp::resolver resolver_{ioc_};
    std::unique_ptr<websocket::stream<beast::ssl_stream<beast::tcp_stream>>> ws_;
    beast::flat_buffer buffer_;
    std::atomic<bool> running_{false};
};