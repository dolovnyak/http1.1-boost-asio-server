#include "Config.h"

namespace {
/// TODO delete when parse will finish
    std::vector<ServerConfig> mock_1_server_config() {
        std::vector<ServerConfig> configs;

        ServerConfig config;
        config.name = "Kabun";
        config.port = 2222;
        config.root_path = "examples/TODO/";
        config.cgi_directory_paths = std::vector<std::string>();
        configs.push_back(config);
        return configs;
    }

/// TODO delete
    std::vector<ServerConfig> mock_2_server_configs() {
        std::vector<ServerConfig> configs;

        ServerConfig config1;
        config1.name = "HelloWorld";
        config1.port = 1337;
        config1.root_path = "examples/hello_world/";
        config1.cgi_directory_paths = std::vector<std::string>();
        configs.push_back(config1);

        ServerConfig config2;
        config2.name = "CgiChecker";
        config2.port = 1488;
        config2.root_path = "examples/cgi_checker/";
        config2.cgi_directory_paths = std::vector<std::string>();
        config2.cgi_directory_paths.push_back("examples/cgi_checker/cgi1/");
        config2.cgi_directory_paths.push_back("examples/cgi_checker/cgi2/"); /// NOTE: hate C++ 98
        configs.push_back(config2);
        return configs;
    }
}


void Config::Load(const char* path) {
    /// TODO delete
    if (std::string(path) == "1") {
        _max_connection_number = 10000;
        _threads_number = 8;
        _max_events_number = 256;
        _servers_configs = mock_1_server_config();
    }
    else if (std::string(path) == "2") {
        _max_connection_number = 10000;
        _threads_number = 8;
        _max_events_number = 128;
        _servers_configs = mock_2_server_configs();
    }
    else {
        /// TODO(Jeka) fill _servers_configs from json
    }
}

uint32_t Config::GetMaxConnectionNumber() const {
    return _max_connection_number;
}

uint32_t Config::GetThreadsNumber() const {
    return _threads_number;
}

uint32_t Config::GetMaxEventsNumber() const {
    return _max_events_number;
}

const std::vector<ServerConfig>& Config::GetServersConfigs() const {
    return _servers_configs;
}
