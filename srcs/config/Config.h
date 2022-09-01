#pragma once

#include "SharedPtr.h"

#include <string>
#include <vector>

struct ServerConfig {
    ServerConfig(int port, const std::string& name, const std::string& root_path,
                 const std::vector<std::string>& cgi_file_extensions,
                 const std::string& default_file_name,
                 int default_keep_alive_timeout,
                 int max_keep_alive_timeout);

    int port;

    std::string name;

    std::string root_path;

    std::vector<std::string> cgi_file_extensions;

    std::string default_file_name;

    int default_keep_alive_timeout;

    int max_keep_alive_timeout;
};

struct Config {
    bool Load(const char* path);

    int max_sockets_number;

    size_t read_buffer_size;

    std::vector<SharedPtr<ServerConfig> > servers_configs;
};
