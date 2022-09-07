#include "Config.h"

bool Config::Load(const char* path) {
    /// TODO delete
    if (std::string(path) == "1") {
        read_buffer_size = 1024;
        max_sockets_number = 256;
        servers_configs = mock_1_server_config();
        sessions_killer_delay_s = 2;
        core_timeout_ms = 1000;

        if (sessions_killer_delay_s * 1000 < core_timeout_ms) {
            throw std::runtime_error("core_timeout should be <= sessions_killer_delay");
        }
        return true;
    }
    else if (std::string(path) == "2") {
        read_buffer_size = 1024;
        max_sockets_number = 512;
        servers_configs = mock_2_server_configs();
        sessions_killer_delay_s = 2;
        core_timeout_ms = 1000;

        if (sessions_killer_delay_s * 1000 < core_timeout_ms) {
            throw std::runtime_error("core_timeout should be <= sessions_killer_delay");
        }
        return true;
    }
    else {
        /// TODO(Jeka) fill servers_configs from json
    }
        threads_number = 8;
        max_sockets_number = 256;
        timeout = 1000;
        servers_configs = mock_1_server_config();
        return true;
    }
    else if (std::string(path) == "2") {
        threads_number = 8;
        max_sockets_number = 128;
        timeout = 1000;
        servers_configs = mock_2_server_configs();
        return true;
    }
    else {
        /// Olga updateeeed =0
        Config config;

        ws_jtoc_setup(config, "conf/default_config.json");
        std::cout << config.threads_number << std::endl;
        std::cout << config.max_sockets_number << std::endl;
        std::cout << config.timeout << std::endl;
        LOG_INFO("name: ", config.servers_configs[0].name);
        std::cout << config.servers_configs[0].port << std::endl;
        LOG_INFO("root_path: ", config.servers_configs[0].root_path);
        std::cout << config.servers_configs[0].cgi_directory_paths[0] << std::endl;
        std::cout << config.servers_configs[0].cgi_directory_paths[1] << std::endl;
        std::cout << config.servers_configs[0].max_connection_number<< std::endl;
        std::cout << config.servers_configs[1].name << std::endl;
        std::cout << config.servers_configs[1].port << std::endl;
        std::cout << config.servers_configs[1].root_path << std::endl;
        std::cout << config.servers_configs[1].cgi_directory_paths[0] << std::endl;
        std::cout << config.servers_configs[1].cgi_directory_paths[1] << std::endl;
        std::cout << config.servers_configs[1].max_connection_number<< std::endl;
    }
    return false;
}

ServerConfig::ServerConfig(int port, const std::string& name, const std::string& root_path,
                           const std::unordered_set<std::string>& cgi_file_extensions,
                           const std::string& default_file_name,
                           int default_keep_alive_timeout_s,
                           int max_keep_alive_timeout_s,
                           int hang_session_timeout_s)
        : port(port), name(name), root_path(root_path), cgi_file_extensions(cgi_file_extensions),
          default_file_name(default_file_name), default_keep_alive_timeout_s(default_keep_alive_timeout_s),
          max_keep_alive_timeout_s(max_keep_alive_timeout_s), hang_session_timeout_s(hang_session_timeout_s) {}
