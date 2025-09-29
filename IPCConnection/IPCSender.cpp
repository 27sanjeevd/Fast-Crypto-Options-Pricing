#include "IPCSender.h"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace IPC {

IPCSender::IPCSender() : client_socket_(-1) {}

IPCSender::~IPCSender() { Disconnect(); }

bool IPCSender::Connect(const std::string &socket_path) {
    if (client_socket_ != -1) {
        Disconnect();
    }

    client_socket_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket_ < 0) {
        std::cerr << "Failed to create client socket: " << strerror(errno) << std::endl;
        return false;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(client_socket_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to connect to server at " << socket_path << ": " << strerror(errno) << std::endl;
        close(client_socket_);
        client_socket_ = -1;
        return false;
    }

    connected_socket_path_ = socket_path;
    std::cout << "Connected to IPC socket: " << socket_path << std::endl;
    return true;
}

bool IPCSender::SendData(const void *data, size_t size) {
    if (client_socket_ == -1) {
        std::cerr << "Not connected to any socket" << std::endl;
        return false;
    }

    // Send size first (4 bytes)
    uint32_t data_size = static_cast<uint32_t>(size);
    if (send(client_socket_, &data_size, sizeof(data_size), 0) < 0) {
        std::cerr << "Failed to send data size: " << strerror(errno) << std::endl;
        return false;
    }

    // Send the actual data
    if (send(client_socket_, data, size, 0) < 0) {
        std::cerr << "Failed to send data: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

void IPCSender::Disconnect() {
    if (client_socket_ != -1) {
        close(client_socket_);
        client_socket_ = -1;
        if (!connected_socket_path_.empty()) {
            std::cout << "Disconnected from IPC socket: " << connected_socket_path_ << std::endl;
            connected_socket_path_.clear();
        }
    }
}

} // namespace IPC
