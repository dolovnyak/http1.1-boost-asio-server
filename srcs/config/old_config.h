
#pragma once

#include <string>
#include <vector>

#define READ_BUFFER_SIZE 1024  // TODO maybe set it in cmake file or set in config and make buffer dynamic

struct ServerConfig {
    ServerConfig(std::string name, uint16_t port, std::string root_path, std::vector<std::string> cgi_directory_paths,
                 int32_t max_connections_number);
    ServerConfig() = default;

    std::string name;
    uint16_t port;
    std::string root_path;
    std::vector<std::string> cgi_directory_paths;
    int32_t max_connection_number;
};

class Config {
public:
    bool Load(const char* path);

    uint32_t threads_number; /// minimum 3
    uint32_t max_sockets_number; /// socket array size for epoll_wait or poll
    int32_t timeout;
    std::vector<ServerConfig> servers_configs;

};