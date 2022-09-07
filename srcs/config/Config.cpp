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
        str.push_back(jtoc_get_string(tmp));
        tmp = tmp->right;
    }
    return(FUNCTION_SUCCESS);
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
