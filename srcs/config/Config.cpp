#include "Config.h"

#include <utility>

namespace {
/// TODO delete when parse will finish
    std::vector<SharedPtr<ServerConfig> > mock_1_server_config() {
        std::vector<SharedPtr<ServerConfig> > configs;

        ServerConfig config(2222, "Kabun", "examples/aaaa/", std::unordered_set<std::string>(), "index.html", 10, 300, 10);
        configs.push_back(MakeShared(config));
        return configs;
    }

/// TODO delete
    std::vector<SharedPtr<ServerConfig> > mock_2_server_configs() {
        std::vector<SharedPtr<ServerConfig> > configs;

        ServerConfig config1(1337, "HelloWorld", "/Users/sbecker/Desktop/projects/webserver-42/", std::unordered_set<std::string>(), "index.html", 60, 300, 20);
        configs.push_back(MakeShared(config1));

        std::unordered_set<std::string> cgi_extensions;
        cgi_extensions.insert(".py");
        cgi_extensions.insert(".php");
        ServerConfig config2(1488, "CgiChecker", "examples/cgi_checker/", cgi_extensions, "index.html", 60, 300, 20);

        configs.push_back(MakeShared(config2));
        return configs;
    }
}


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
