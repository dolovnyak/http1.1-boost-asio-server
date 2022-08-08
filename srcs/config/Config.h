#pragma once

#include <string>
#include <vector>

struct ServerConfig {
    ServerConfig(std::string name, uint16_t port, std::string root_path, std::vector<std::string> cgi_directory_paths,
                 int32_t max_connections_number);

    std::string name;
    uint16_t port;
    std::string root_path;
    std::vector<std::string> cgi_directory_paths;
    int32_t max_connection_number;
};

class Config {
public:
    bool Load(const char* path);

    uint32_t GetThreadsNumber() const;

    uint32_t GetMaxEventsNumber() const;

    int32_t GetTimeout() const;

    const std::vector<ServerConfig>& GetServersConfigs() const;

private:
    uint32_t _threads_number; /// minimum 3
    uint32_t _max_events_number; /// max events from epoll_wait or poll
    int32_t _timeout;
    std::vector<ServerConfig> _servers_configs;
};
