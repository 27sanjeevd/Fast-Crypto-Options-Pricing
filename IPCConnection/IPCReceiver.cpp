#include "IPCReceiver.h"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace IPC {

IPCReceiver::IPCReceiver(const std::string &socket_path) : socket_path_(socket_path), server_socket_(-1), client_socket_(-1) {}

IPCReceiver::~IPCReceiver() { CleanupSocket(); }

bool IPCReceiver::Initialize() {
    unlink(socket_path_.c_str());

    server_socket_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return false;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path_.c_str(), sizeof(addr.sun_path) - 1);

    if (bind(server_socket_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind socket: " << strerror(errno) << std::endl;
        CleanupSocket();
        return false;
    }

    if (listen(server_socket_, 1) < 0) {
        std::cerr << "Failed to listen on socket: " << strerror(errno) << std::endl;
        CleanupSocket();
        return false;
    }

    std::cout << "IPC socket listening on: " << socket_path_ << std::endl;
    return true;
}

std::vector<uint8_t> IPCReceiver::ReadData() {
    if (server_socket_ == -1) {
        std::cerr << "Socket not initialized" << std::endl;
        return {};
    }

    if (client_socket_ == -1) {
        if (!AcceptConnection()) {
            return {};
        }
    }

    // Read data length first (4 bytes)
    uint32_t data_length;
    if (!ReadExact(&data_length, sizeof(data_length))) {
        return {};
    }

    // Read the actual data
    std::vector<uint8_t> data(data_length);
    if (!ReadExact(data.data(), data_length)) {
        return {};
    }

    return data;
}

bool IPCReceiver::ReadExact(void *buffer, size_t size) {
    size_t bytes_remaining = size;
    uint8_t *buf_ptr = static_cast<uint8_t *>(buffer);

    while (bytes_remaining > 0) {
        ssize_t bytes_read = recv(client_socket_, buf_ptr, bytes_remaining, 0);
        if (bytes_read <= 0) {
            if (bytes_read == 0) {
                std::cout << "Client disconnected" << std::endl;
            } else {
                std::cerr << "Failed to read data: " << strerror(errno) << std::endl;
            }
            close(client_socket_);
            client_socket_ = -1;
            return false;
        }

        bytes_remaining -= bytes_read;
        buf_ptr += bytes_read;
    }

    return true;
}

bool IPCReceiver::AcceptConnection() {
    client_socket_ = accept(server_socket_, nullptr, nullptr);
    if (client_socket_ < 0) {
        std::cerr << "Failed to accept connection: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "Client connected to IPC socket" << std::endl;
    return true;
}

void IPCReceiver::CleanupSocket() {
    if (client_socket_ != -1) {
        close(client_socket_);
        client_socket_ = -1;
    }
    if (server_socket_ != -1) {
        close(server_socket_);
        server_socket_ = -1;
    }
    unlink(socket_path_.c_str());
}

} // namespace IPC
