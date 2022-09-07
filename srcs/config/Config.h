#pragma once

#include "SharedPtr.h"

#include <string>
#include <vector>
#include <unordered_set>

struct ServerConfig {
    ServerConfig(int port, const std::string& name, const std::string& root_path,
                 const std::unordered_set<std::string>& cgi_file_extensions,
                 const std::string& default_file_name,
                 int default_keep_alive_timeout_s,
                 int max_keep_alive_timeout_s,
                 int hang_session_timeout_s);

    int port;

    std::string name;

    std::string root_path;

    std::unordered_set<std::string> cgi_file_extensions;

    std::string default_file_name;

    int default_keep_alive_timeout_s;

    int max_keep_alive_timeout_s;

    int hang_session_timeout_s;
};

struct Config {
    bool Load(const char* path);

    int max_sockets_number;

    size_t read_buffer_size;

    int sessions_killer_delay_s;

    int core_timeout_ms;  /// this value conflict with sessions_killer_delay_s, so this value should be <= sessions_killer_delay_s (set exception on ths when load config)

    std::vector<SharedPtr<ServerConfig> > servers_configs;
};

