#pragma once

#include <atomic>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <chrono>
#include <memory>
#include <string>
#include <thread>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = net::ip::tcp;

class ExchangeWebsocketClient {
  public:
    ExchangeWebsocketClient();
    ~ExchangeWebsocketClient();

    void start();
    void stop();
    bool isRunning() const { return running_; }
    void subscribeTo(const std::string &channel);

  private:
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void on_connect(beast::error_code ec,
                    tcp::resolver::results_type::endpoint_type ep);
    void on_ssl_handshake(beast::error_code ec);
    void on_handshake(beast::error_code ec);
    void do_read();
    void on_read(beast::error_code ec, std::size_t bytes_transferred);

    const std::string host_ = "stream.crypto.com";
    const std::string port_ = "443";
    const std::string target_ = "/exchange/v1/market";

    net::io_context ioc_;
    ssl::context ctx_{ssl::context::tlsv12_client};
    tcp::resolver resolver_{ioc_};
    std::unique_ptr<websocket::stream<beast::ssl_stream<beast::tcp_stream>>>
        ws_;
    beast::flat_buffer buffer_;
    std::atomic<bool> running_{false};

    std::string currency_pair_;
};