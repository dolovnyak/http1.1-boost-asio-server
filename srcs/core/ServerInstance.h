#pragma once

#include <cstdint>
#include <string>
#include <utility>

class ServerInstance {
public:
    ServerInstance(int32_t listen_socket_fd, std::string name)
    : listening_socket_fd(listen_socket_fd),
      name(std::move(name)) {}

      ServerInstance(const ServerInstance&) = delete;

    int32_t listening_socket_fd;
    std::string name;
    /// maybe it will be contain name, root path and cgi directory paths
};