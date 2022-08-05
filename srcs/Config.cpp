#include "Config.h"

namespace {
/// TODO delete when parse will finish
    std::vector<Config> mock_1_config() {
        std::vector<Config> configs;

        Config config;
        config.name = "Kabun";
        config.port = 2222;
        config.root_path = "examples/TODO/";
        config.cgi_directory_paths = std::vector<std::string>();
        config.max_connection_number = 10000;
        config.threads_number = 8;
        config.max_events_number = 256;
        configs.push_back(config);
        return configs;
    }

/// TODO delete
    std::vector<Config> mock_2_configs() {
        std::vector<Config> configs;

        Config config1;
        config1.name = "HelloWorld";
        config1.port = 1337;
        config1.root_path = "examples/hello_world/";
        config1.cgi_directory_paths = std::vector<std::string>();
        config1.max_connection_number = 100;
        config1.threads_number = 3;
        config1.max_events_number = 128;
        configs.push_back(config1);

        Config config2;
        config2.name = "CgiChecker";
        config2.port = 1488;
        config2.root_path = "examples/cgi_checker/";
        config2.cgi_directory_paths = std::vector<std::string>();
        config2.cgi_directory_paths.push_back("examples/cgi_checker/cgi1/");
        config2.cgi_directory_paths.push_back("examples/cgi_checker/cgi2/"); /// NOTE: hate C++ 98
        config2.max_connection_number = 1000;
        config2.threads_number = 5;
        config2.max_events_number = 256;
        configs.push_back(config2);
        return configs;
    }
}

void CommonConfig::Load(const char* path) {
    /// TODO delete
    if (std::string(path) == "1") {
        _configs = mock_1_config();
    }
    else if (std::string(path) == "2") {
        _configs = mock_2_configs();
    }
    else {
        /// TODO(Jeka) fill _configs from json
    }
}