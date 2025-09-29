#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace IPC {

class IPCReceiver {
  public:
    IPCReceiver(const std::string &socket_name);
    ~IPCReceiver();

    bool Initialize();

    std::vector<uint8_t> ReadData();

    bool IsInitialized() const { return server_socket_ != -1; }

    const std::string &GetSocketPath() const { return socket_path_; }

  private:
    void CleanupSocket();
    bool AcceptConnection();
    bool ReadExact(void *buffer, size_t size);

    std::string socket_name_;
    std::string socket_path_;
    int server_socket_;
    int client_socket_;
};

} // namespace IPC
