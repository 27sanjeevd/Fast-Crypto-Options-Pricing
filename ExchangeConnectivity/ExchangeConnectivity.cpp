#include "ExchangeConnectivity.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <sstream>
#include <thread>

ExchangeWebsocketClient::ExchangeWebsocketClient() : running_(false) {
    ctx_.set_verify_mode(ssl::verify_peer);
    ctx_.set_default_verify_paths();

    ws_ = std::make_unique<
        websocket::stream<beast::ssl_stream<beast::tcp_stream>>>(ioc_, ctx_);
}

ExchangeWebsocketClient::~ExchangeWebsocketClient() { stop(); }

void ExchangeWebsocketClient::start() {
    if (running_)
        return;
    running_ = true;

    if (!SSL_set_tlsext_host_name(ws_->next_layer().native_handle(),
                                  host_.c_str())) {
        throw beast::system_error(
            beast::error_code(static_cast<int>(::ERR_get_error()),
                              net::error::get_ssl_category()));
    }

    resolver_.async_resolve(
        host_, port_,
        beast::bind_front_handler(&ExchangeWebsocketClient::on_resolve, this));

    std::thread([this]() {
        try {
            ioc_.run();
        } catch (const std::exception &e) {
            std::cerr << "IO service error: " << e.what() << std::endl;
        }
    }).detach();
}

void ExchangeWebsocketClient::stop() {
    if (!running_)
        return;
    running_ = false;

    beast::error_code ec;
    if (ws_) {
        ws_->close(websocket::close_code::normal, ec);
    }
    ioc_.stop();
}

void ExchangeWebsocketClient::on_resolve(beast::error_code ec,
                                         tcp::resolver::results_type results) {
    if (ec) {
        std::cerr << "Error resolving: " << ec.message() << std::endl;
        return;
    }

    beast::get_lowest_layer(*ws_).expires_after(std::chrono::seconds(30));

    beast::get_lowest_layer(*ws_).async_connect(
        results,
        beast::bind_front_handler(&ExchangeWebsocketClient::on_connect, this));
}

void ExchangeWebsocketClient::on_connect(
    beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
    if (ec) {
        std::cerr << "Error connecting: " << ec.message() << std::endl;
        return;
    }

    beast::get_lowest_layer(*ws_).expires_after(std::chrono::seconds(30));

    ws_->next_layer().async_handshake(
        ssl::stream_base::client,
        beast::bind_front_handler(&ExchangeWebsocketClient::on_ssl_handshake,
                                  this));
}

void ExchangeWebsocketClient::on_ssl_handshake(beast::error_code ec) {
    if (ec) {
        std::cerr << "Error SSL handshake: " << ec.message() << std::endl;
        return;
    }

    beast::get_lowest_layer(*ws_).expires_never();

    ws_->set_option(
        websocket::stream_base::timeout::suggested(beast::role_type::client));

    ws_->set_option(
        websocket::stream_base::decorator([](websocket::request_type &req) {
            req.set(http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-client-async");
        }));

    ws_->async_handshake(host_, target_,
                         beast::bind_front_handler(
                             &ExchangeWebsocketClient::on_handshake, this));
}

void ExchangeWebsocketClient::on_handshake(beast::error_code ec) {
    if (ec) {
        std::cerr << "Error websocket handshake: " << ec.message() << std::endl;
        return;
    }

    std::cout << "WebSocket handshake successful!" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    subscribeTo("book.BTCUSD-PERP.10");

    do_read();
}

void ExchangeWebsocketClient::do_read() {
    ws_->async_read(buffer_, beast::bind_front_handler(
                                 &ExchangeWebsocketClient::on_read, this));
}

void ExchangeWebsocketClient::on_read(beast::error_code ec,
                                      std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) {
        if (ec != websocket::error::closed) {
            std::cerr << "Error reading: " << ec.message() << std::endl;
        }
        return;
    }

    std::string msg = beast::buffers_to_string(buffer_.data());
    std::cout << "Received message: " << msg << std::endl;

    buffer_.consume(buffer_.size());

    if (running_) {
        do_read();
    }
}

void ExchangeWebsocketClient::subscribeTo(const std::string &channel) {
    if (!ws_ || !running_) {
        std::cerr << "WebSocket not connected, cannot subscribe" << std::endl;
        return;
    }

    boost::property_tree::ptree subscription;
    subscription.put("id", 1);
    subscription.put("method", "subscribe");

    boost::property_tree::ptree channels;
    boost::property_tree::ptree channel_array;
    channel_array.put("", channel);
    channels.push_back(std::make_pair("", channel_array));

    boost::property_tree::ptree params;
    params.add_child("channels", channels);

    params.put("book_subscription_type", "SNAPSHOT_AND_UPDATE");

    subscription.add_child("params", params);

    subscription.put("nonce",
                     std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count());

    std::ostringstream oss;
    boost::property_tree::write_json(oss, subscription);
    std::string json_msg = oss.str();

    std::cout << "Sending subscription (snapshot + deltas): " << json_msg
              << std::endl;

    ws_->async_write(net::buffer(json_msg), [](beast::error_code ec,
                                               std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (ec) {
            std::cerr << "Error sending subscription: " << ec.message()
                      << std::endl;
        } else {
            std::cout << "Subscription sent successfully!" << std::endl;
        }
    });
}
