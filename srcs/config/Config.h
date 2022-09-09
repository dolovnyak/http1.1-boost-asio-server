#pragma once

#include "SharedPtr.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>


//place for default vars?

struct LocationsConfig{
    LocationsConfig(
        std::string location,
        std::string root,
        bool autoindex,
        std::string index,
        std::unordered_set<std::string> availableMethod,
        std::string redirect);

    LocationsConfig() = default;

    std::string location;

    std::string root;

    bool autoindex;

    std::string index;

    std::unordered_set<std::string> availableMethod;

    std::string redirect; 

};

struct ServerConfig {
    ServerConfig(int port,
                const std::string& name,
                const std::string& root_path,
                const std::unordered_set<std::string>& cgi_file_extensions,
                const std::string& default_file_name,
                int max_body_size,
                int max_header_size,
                int max_raw_request_size,
                int default_keep_alive_timeout_s,
                int max_keep_alive_timeout_s,
                int hang_session_timeout_s);

    ServerConfig() = default;

    int port;

    std::string name;

    std::string root_path;

    std::string default_file_name; // not in tmp config

    std::unordered_map <int, std::string> defauls_error_pages;

    std::unordered_set<std::string> cgi_file_extensions;

    int default_keep_alive_timeout_s;

    int max_keep_alive_timeout_s;

    int hang_session_timeout_s;

    std::vector<SharedPtr<LocationsConfig> > locations;
};

struct Config {
    bool Load(const char* path);

    int max_sockets_number;

    size_t read_buffer_size;

    int sessions_killer_delay_s;

    int core_timeout_ms;  /// this value conflict with sessions_killer_delay_s, so this value should be <= sessions_killer_delay_s (set exception on ths when load config)

    std::vector<SharedPtr<ServerConfig>> servers_configs;
};

