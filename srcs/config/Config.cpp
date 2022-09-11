#include "Config.h"
#include "parse_config.h"

namespace {
/// TODO delete when parse will finish

    // std::vector<SharedPtr<ServerConfig> > mock_1_server_config() {
    //     std::vector<SharedPtr<ServerConfig> > configs;

    //     ServerConfig config(2222, "Kabun", "examples/aaaa/", std::unordered_set<std::string>(), "index.html", 10, 300, 10);
    //     configs.push_back(MakeShared(config));
    //     return configs;
    // }

/// TODO delete

    // std::vector<SharedPtr<ServerConfig> > mock_2_server_configs() {
//         std::vector<SharedPtr<ServerConfig> > configs;
//
//         ServerConfig config1(1337, "HelloWorld", "/Users/sbecker/Desktop/projects/webserver-42/", std::unordered_set<std::string>(), "index.html", 60, 300, 20);
//         configs.push_back(MakeShared(config1));

    //     std::unordered_set<std::string> cgi_extensions;
    //     cgi_extensions.insert(".py");
    //     cgi_extensions.insert(".php");
    //     ServerConfig config2(1488, "CgiChecker", "examples/cgi_checker/", cgi_extensions, "index.html", 60, 300, 20);

    //     configs.push_back(MakeShared(config2));
    //     return configs;
    // }
}

bool Config::Load(const char* path) {
    /// TODO delete
    // if (std::string(path) == "1") {
    //     threads_number = 8;
    //     max_sockets_number = 256;
    //     timeout = 1000;
    //     servers_configs = mock_1_server_config();
    //     return true;
    // }
    // else if (std::string(path) == "2") {
    //     threads_number = 8;
    //     max_sockets_number = 128;
    //     timeout = 1000;
    //     servers_configs = mock_2_server_configs();
    //     return true;
    //     //         str.push_back(jtoc_get_string(tmp));
    //     //         tmp = tmp->right;
    // }
    // else {
        /// Olga updateeeed =0
    Config config;

    if(ws_jtoc_setup(config, "conf/default_config.json") == FUNCTION_SUCCESS) { // change 2 arg to path
        LOG_INFO("max_sockets_number: ", config.max_sockets_number);
        // LOG_INFO("max sockets_number: ", config.max_sockets_number);
        // LOG_INFO("timeout: ", config.timeout);
        // LOG_INFO("servers_configs_0 name: ", config.servers_configs[0].name);
        // LOG_INFO("servers_configs_0 port: ", config.servers_configs[0].port);
        // LOG_INFO("servers_configs_0 root: ", config.servers_configs[0].root);
        // LOG_INFO("servers_configs_0 cgi_directory_paths_0: ", config.servers_configs[0].cgi_directory_paths[0]);
        // LOG_INFO("servers_configs_0 cgi_directory_paths_1: ", config.servers_configs[0].cgi_directory_paths[1]);
        // LOG_INFO("servers_configs_0 cgi_directory_paths_2: ", config.servers_configs[0].cgi_directory_paths[2]);
        // LOG_INFO("servers_configs_0 max_connection_number: ", config.servers_configs[0].max_connection_number);
        // LOG_INFO("servers_configs_1 name: ", config.servers_configs[1].name);
        // LOG_INFO("servers_configs_1 port: ", config.servers_configs[1].port);
        // LOG_INFO("servers_configs_1 root: ", config.servers_configs[1].root);
        // LOG_INFO("servers_configs_1 cgi_directory_paths_0: ", config.servers_configs[1].cgi_directory_paths[0]);
        // LOG_INFO("servers_configs_1 cgi_directory_paths_1: ", config.servers_configs[1].cgi_directory_paths[1]);
        // LOG_INFO("servers_configs_1 max_connection_number: ", config.servers_configs[1].max_connection_number);
    return false;
    }
    return true;
}

ServerConfig::ServerConfig(int port,
                            const std::string& name,
                            const std::string& root,
                            std::unordered_map <int, std::string> error_pages,
                            const std::unordered_set<std::string>& cgi_file_extensions,
                            int default_keep_alive_timeout_s,
                            int max_keep_alive_timeout_s, 
                            std::vector<Location> locations
                            )
                    : port(port),
                    name(name),
                    root(root),
                    error_pages(error_pages), 
                    cgi_file_extensions(cgi_file_extensions),
                    default_keep_alive_timeout_s(default_keep_alive_timeout_s),
                    max_keep_alive_timeout_s(max_keep_alive_timeout_s),
                    locations(locations){}



