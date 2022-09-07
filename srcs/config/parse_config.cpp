#include "Config.h"
#include "libjtoc.h"
#include "Logging.h"

int ws_jtoc_get_config(ServerConfig& config,
                       t_jnode	*n)
{
    t_jnode	*tmp;
    //std::vector<std::string> cgi_directory_paths;

    //  get Name
    if (!(tmp = jtoc_node_get_by_path(n, "Name"))
        || tmp->type != string) {
        LOG_ERROR("Failed to read json ServerInstances Name");
        return (FUNCTION_FAILURE);
    }
    config.name = jtoc_get_string(tmp);

    // get port
    if (!(tmp = jtoc_node_get_by_path(n, "Port"))
        || tmp->type != integer) {
        LOG_ERROR("Failed to read json ServerInstances Port");
        return (FUNCTION_FAILURE);
    }
    config.port = jtoc_get_int(tmp);

    //  get Root
    if (!(tmp = jtoc_node_get_by_path(n, "Root"))
        || tmp->type != string) {
        LOG_ERROR("Failed to read json ServerInstances Root");
        return (FUNCTION_FAILURE);
    }
    config.root_path = jtoc_get_string(tmp);

    //  get MaxCoonnectionNumber
    if (!(tmp = jtoc_node_get_by_path(n, "MaxCoonnectionNumber"))
        || tmp->type != integer) {
        LOG_ERROR("Failed to read json ServerInstances MaxCoonnectionNumber");
        return (FUNCTION_FAILURE);
    }
    config.max_connection_number = jtoc_get_int(tmp);

    //get CgiDirectories
    if (!(tmp = jtoc_node_get_by_path(n, "CgiDirectories"))
        || tmp->type != array) {
        LOG_ERROR("Failed to read json ServerInstances CgiDirectories");
        return (FUNCTION_FAILURE);
    }
    if (ws_jtoc_get_servers_config(config.cgi_directory_paths, tmp) == FUNCTION_FAILURE) {

        return (FUNCTION_FAILURE);
    }
    return (FUNCTION_SUCCESS);
}

int ws_jtoc_get_servers_configs(std::vector<ServerConfig>& server_configs,
                                t_jnode	*n)
{
    t_jnode         *tmp;
    ServerConfig config;

    tmp = n->down;
    while(tmp) {
        if (tmp->type != object) {
            LOG_ERROR("Failed to read json array ServerInstances");
            return (FUNCTION_FAILURE);
        }
        if (ws_jtoc_get_config(config, tmp) == FUNCTION_FAILURE) {
            return (FUNCTION_FAILURE);
        }
        server_configs.push_back(config);
        tmp = tmp->right;
    }
    return (FUNCTION_SUCCESS);
}

int				ws_jtoc_setup(Config& config,
                                 const char *json)
{
    t_jnode	*root;
    t_jnode	*tmp;

    if (!(root = jtoc_read(json))) {
        LOG_ERROR("Failed to read json");
        return (FUNCTION_FAILURE);
    }
    //  get ThreadsNumber
    if (!(tmp = jtoc_node_get_by_path(root, "ThreadsNumber"))
        || tmp->type != integer) {
        LOG_ERROR("Failed to read json ThreadsNumber");
        return (FUNCTION_FAILURE);
    }
    config.threads_number = jtoc_get_int(tmp);

    //get MaxSocketsNumber
    if (!(tmp = jtoc_node_get_by_path(root, "MaxSocketsNumber"))
        || tmp->type != integer) {
        LOG_ERROR("Failed to read json MaxSocketsNumber");
        return (FUNCTION_FAILURE);
    }
    config.max_sockets_number = jtoc_get_int(tmp);

    //get Timeout
    if (!(tmp = jtoc_node_get_by_path(root, "Timeout"))
        || tmp->type != integer) {
        LOG_ERROR("Failed to read json Timeout");
        return (FUNCTION_FAILURE);
    }
    config.timeout = jtoc_get_int(tmp);

    //get ServerInstances
    if (!(tmp = jtoc_node_get_by_path(root, "ServerInstances"))
        || tmp->type != array) {
        LOG_ERROR("Failed to read json ServerInstances");
        return (FUNCTION_FAILURE);
    }
    return(ws_jtoc_get_servers_configs(config.servers_configs, tmp));
}

namespace {
/// TODO delete when parse will finish
    std::vector<ServerConfig> mock_1_server_config() {
        std::vector<ServerConfig> configs;

        ServerConfig config("Kabun", 2222, "examples/TODO", {}, 10000);
        configs.push_back(config);
        return configs;
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
