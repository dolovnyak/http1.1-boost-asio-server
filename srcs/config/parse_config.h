#include "Logging.h"
#include "Config.h"

#include "libjtoc.h"
#include <utility>

int ws_jtoc_get_servers_config(std::vector<std::string>& str,
                               t_jnode	*n)
{
    t_jnode			*tmp;

    tmp = n->down;
    while (tmp)
    {
        if (tmp->type != string){
            LOG_ERROR("Failed to read json ServerInstances CgiDirectoriesv array");
            return (FUNCTION_FAILURE);
        }
        str.push_back(jtoc_get_string(tmp));
        tmp = tmp->right;
    }
    (FUNCTION_SUCCESS);
}

int ws_jtoc_get_servers_config(ServerConfig& config,
                               t_jnode	*n)
{
    t_jnode	*tmp;
    std::vector<std::string> cgi_directory_paths;

    //  get Name
    if (!(tmp = jtoc_node_get_by_path(n, "Name"))
        || tmp->type != string) {
        LOG_ERROR("Failed to read json ServerInstances Name");
        return (FUNCTION_FAILURE);
    }
    LOG_INFO("check: ", jtoc_get_string(tmp));
    config.name = jtoc_get_string(tmp);
    LOG_INFO("check name: ", config.name);

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
    if (!(tmp = jtoc_node_get_by_path(n, "MaxConnectionNumber"))
        || tmp->type != integer) {
        LOG_ERROR("Failed to read json ServerInstances MaxConnectionNumber");
        return (FUNCTION_FAILURE);
    }
    config.max_sockets_number = jtoc_get_int(tmp);

    //get CgiDirectories
    if (!(tmp = jtoc_node_get_by_path(n, "CgiDirectories"))
        || tmp->type != array) {
        LOG_ERROR("Failed to read json ServerInstances CgiDirectories");
        return (FUNCTION_FAILURE);
    }
    if (ws_jtoc_get_servers_config(cgi_directory_paths, tmp) == FUNCTION_FAILURE) {

        return (FUNCTION_FAILURE);
    }
    return (FUNCTION_SUCCESS);
}

int ws_jtoc_get_servers_configs(std::vector<ServerConfig>& server_configs,
                                t_jnode	*n)
{
    t_jnode         *tmp;
    ServerConfig config; //???
    unsigned int	g;

    tmp = n->down;
    while(tmp) {
        if (tmp->type != object) {
            LOG_ERROR("Failed to read json array ServerInstances");
            return (FUNCTION_FAILURE);
        }
        if (ws_jtoc_get_servers_config(config, tmp) == FUNCTION_FAILURE) {
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
    LOG_INFO("Check");
    return(ws_jtoc_get_servers_configs(config.servers_configs, tmp));
}

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

