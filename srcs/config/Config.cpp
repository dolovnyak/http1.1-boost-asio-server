#include "Config.h"

#include <utility>

namespace {
/// TODO delete when parse will finish
    std::vector<ServerConfig> mock_1_server_config() {
        std::vector<ServerConfig> configs;

        ServerConfig config("Kabun", 2222, "examples/TODO", std::vector<std::string>(), 10000);
        configs.push_back(config);
        return configs;
    }

/// TODO delete
    std::vector<ServerConfig> mock_2_server_configs() {
        std::vector<ServerConfig> configs;

        ServerConfig config1("HelloWorld", 1337, "examples/hello_world/", std::vector<std::string>(), 1000);
        configs.push_back(config1);

        std::vector<std::string> cgi_paths;
        cgi_paths.push_back("examples/cgi_checker/cgi1/");
        cgi_paths.push_back("examples/cgi_checker/cgi2/");
        ServerConfig config2("CgiChecker", 1488, "examples/cgi_checker/", cgi_paths, 1000);

        configs.push_back(config2);
        return configs;
    }
}


bool Config::Load(const char* path) {
    /// TODO delete
    if (std::string(path) == "1") {
        _threads_number = 8;
        _max_sockets_number = 256;
        _timeout = 1000;
        _servers_configs = mock_1_server_config();
        return true;
    }
    else if (std::string(path) == "2") {
        _threads_number = 8;
        _max_sockets_number = 128;
        _timeout = 1000;
        _servers_configs = mock_2_server_configs();
        return true;
    }
    else {
        /// TODO(Jeka) fill _servers_configs from json
    }
    return false;
}

uint32_t Config::GetThreadsNumber() const {
    return _threads_number;
}

/// TODO make max events number and max sockets number
uint32_t Config::GetMaxEventsNumber() const {
    return _max_sockets_number;
}

const std::vector<ServerConfig>& Config::GetServersConfigs() const {
    return _servers_configs;
}

int32_t Config::GetTimeout() const {
    return _timeout;
}

ServerConfig::ServerConfig(std::string name, uint16_t port, std::string root_path,
                           std::vector<std::string> cgi_directory_paths,
                           int32_t max_connection_number)
        : name(std::move(name)), port(port), root_path(std::move(root_path)),
          cgi_directory_paths(std::move(cgi_directory_paths)),
          max_connection_number(max_connection_number) {}
