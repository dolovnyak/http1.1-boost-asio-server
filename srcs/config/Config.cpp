#include "Config.h"

bool Config::Load(const char* path) {
    /// TODO delete
    if (std::string(path) == "1") {
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
        str.push_back(jtoc_get_string(tmp));
        tmp = tmp->right;
    }
    return(FUNCTION_SUCCESS);
}

    else {
        /// Olga updateeeed =0
        Config config;

        if(ws_jtoc_setup(config, "conf/default_config.json") == FUNCTION_SUCCESS) { // change 2 arg to path
            LOG_INFO("threads_number: ", config.threads_number);
            LOG_INFO("max sockets_number: ", config.max_sockets_number);
            LOG_INFO("timeout: ", config.timeout);
            LOG_INFO("servers_configs_0 name: ", config.servers_configs[0].name);
            LOG_INFO("servers_configs_0 port: ", config.servers_configs[0].port);
            LOG_INFO("servers_configs_0 root_path: ", config.servers_configs[0].root_path);
            LOG_INFO("servers_configs_0 cgi_directory_paths_0: ", config.servers_configs[0].cgi_directory_paths[0]);
            LOG_INFO("servers_configs_0 cgi_directory_paths_1: ", config.servers_configs[0].cgi_directory_paths[1]);
            LOG_INFO("servers_configs_0 cgi_directory_paths_2: ", config.servers_configs[0].cgi_directory_paths[2]);
            LOG_INFO("servers_configs_0 max_connection_number: ", config.servers_configs[0].max_connection_number);
            LOG_INFO("servers_configs_1 name: ", config.servers_configs[1].name);
            LOG_INFO("servers_configs_1 port: ", config.servers_configs[1].port);
            LOG_INFO("servers_configs_1 root_path: ", config.servers_configs[1].root_path);
            LOG_INFO("servers_configs_1 cgi_directory_paths_0: ", config.servers_configs[1].cgi_directory_paths[0]);
            LOG_INFO("servers_configs_1 cgi_directory_paths_1: ", config.servers_configs[1].cgi_directory_paths[1]);
            LOG_INFO("servers_configs_1 max_connection_number: ", config.servers_configs[1].max_connection_number);
        }  
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
