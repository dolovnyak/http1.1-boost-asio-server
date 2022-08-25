#pragma once

#include <cstdint>
#include <string>
#include <utility>

class ServerInfo {
public:
    ServerInfo(int listen_tcp_socket, std::string name)
    : listening_tcp_socket(listen_tcp_socket),
      name(std::move(name)) {}

//      ServerInfo(const ServerInfo&) = delete;

    int listening_tcp_socket;
    std::string name;
    /// maybe it will be contain name, root path and cgi directory paths
};