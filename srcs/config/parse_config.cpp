#include "parse_config.h"
#include "Config.h"
#include "libjtoc.h"
#include "Logging.h"


//add default number
int             ws_jtoc_get_int(const char *key, int *value, t_jnode *n)
{
    t_jnode	*tmp;
    std::string error;

    error += "Failed to read json ";
    error += key;
    if (!(tmp = jtoc_node_get_by_path(n, key))
        || tmp->type != integer) {
        LOG_ERROR(error);
        return (FUNCTION_FAILURE);
    }
    *value = jtoc_get_int(tmp);
    return(FUNCTION_SUCCESS);
}

int ws_jtoc_get_server_config(SharedPtr<ServerConfig>& config,
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
    config.
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

int ws_jtoc_get_server_config_array(std::vector<SharedPtr<ServerConfig>> & server_configs,
                                t_jnode	*n)
{
    t_jnode         *tmp;
    SharedPtr<ServerConfig> config;

    tmp = n->down;
    while(tmp) {
        if (tmp->type != object) {
            LOG_ERROR("Failed to read json array ServerInstances");
            return (FUNCTION_FAILURE);
        }
        if (ws_jtoc_get_server_config(config, tmp) == FUNCTION_FAILURE) {
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

    if (ws_jtoc_extract_int("MaxSocketsNumber", &config.max_sockets_number, root) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    //! change size_t
    // if (ws_jtoc_extract_int("ReadBufferSize", &config.read_buffer_size, root) == FUNCTION_FAILURE){
    //     return (FUNCTION_FAILURE); 
    // }

    if (ws_jtoc_extract_int("SessionsKillerDelay_s", &config.sessions_killer_delay_s, root) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_extract_int("CoreTimeout_s", &config.core_timeout_ms, root) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    //get ServerInstances
    if (!(tmp = jtoc_node_get_by_path(root, "ServerInstances"))
        || tmp->type != array) {
        LOG_ERROR("Failed to read json ServerInstances");
        return (FUNCTION_FAILURE);
    }
    return(ws_jtoc_get_server_config_array(config.servers_configs, tmp));
}