#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace IPC {

class IPCSender {
  public:
    IPCSender();
    ~IPCSender();

    bool Connect(const std::string &socket_path);

    bool SendData(const void *data, size_t size);

    void Disconnect();

    bool IsConnected() const { return client_socket_ != -1; }

  private:
    int client_socket_;
    std::string connected_socket_path_;
};

} // namespace IPC
