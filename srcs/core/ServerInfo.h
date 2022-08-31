#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <unordered_set>

class ServerInfo {
public:
    ServerInfo(int listen_tcp_socket, const std::string& name)
    : listening_tcp_socket(listen_tcp_socket),
      name(name) {}

//      ServerInfo(const ServerInfo&) = delete;

    int listening_tcp_socket;
    std::string name;
    std::string default_file_name;
    std::string root_path;
    std::unordered_set<std::string> cgi_directory_paths;
    /// maybe it will be contain name, root path and cgi directory paths
};