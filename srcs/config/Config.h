#pragma once

#include <string>
#include <vector>

struct ServerConfig {
    std::string name;
    uint16_t port;
    std::string root_path;
    std::vector<std::string> cgi_directory_paths;
};

class Config {
public:
    void Load(const char* path);

    uint32_t GetMaxConnectionNumber() const;

    uint32_t GetThreadsNumber() const;

    uint32_t GetMaxEventsNumber() const;

    const std::vector<ServerConfig>& GetServersConfigs() const;

private:
    uint32_t _max_connection_number;
    uint32_t _threads_number; /// minimum 3
    uint32_t _max_events_number; /// max events from epoll_wait or poll
    std::vector<ServerConfig> _servers_configs;
};
