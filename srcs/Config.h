#pragma once

#include <string>
#include <vector>

struct Config {
    std::string name;
    uint16_t port;
    std::string root_path;
    std::vector<std::string> cgi_directory_paths;
    uint32_t max_connection_number;
    uint32_t threads_number; /// minimum 3
    uint32_t max_events_number; /// max events from epoll_wait or poll (if epoll and poll config will be different create different structures for them)
};

class CommonConfig {
public:
    void Load(const char* path);

private:
    std::vector<Config> _configs;
};
