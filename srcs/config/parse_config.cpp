// #include "parse_config.h"
#include "Config.h"
#include "libjtoc.h"
#include "Logging.h"

int             ws_jtoc_get_int(const char *key, int *value, t_jnode *n, int default_int, bool mandatory_flag)
{
    t_jnode	*tmp;
    std::string error;

    error += "Failed to read json ";
    error += key;
    if (tmp = jtoc_node_get_by_path(n, key)){
        if (tmp->type != integer){
            LOG_ERROR(error);
            return (FUNCTION_FAILURE);
        }
        *value = jtoc_get_int(tmp);
        if (*value <= 0){
            LOG_ERROR(error);
            return (FUNCTION_FAILURE);
        }
        return(FUNCTION_SUCCESS);
    }
    else if (mandatory_flag == true){
        LOG_ERROR(error);
        return (FUNCTION_FAILURE);
    }
    *value = default_int;
    return(FUNCTION_SUCCESS);
}

int ws_jtoc_get_server_config(ServerConfig& server_config,
                       t_jnode	*n)
{
    t_jnode	*tmp;
    std::vector<std::string> cgi_directory_paths;

    if (ws_jtoc_get_int("Port", &server_config.port, n, -1, true) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }
    //  get Name
    if (!(tmp = jtoc_node_get_by_path(n, "Name"))
        || tmp->type != string) {
        LOG_ERROR("Failed to read json ServerInstances Name");
        return (FUNCTION_FAILURE);
    }
    server_config.name = jtoc_get_string(tmp);

    //  get root
    if (!(tmp = jtoc_node_get_by_path(n, "Root"))
        || tmp->type != string) {
        LOG_ERROR("Failed to read json ServerInstances Root");
        return (FUNCTION_FAILURE);
    }
    /// TODO add '/' 
    server_config.root = jtoc_get_string(tmp);

    if (ws_jtoc_get_int("DefaultKeepAliveTimeout_s", &server_config.default_keep_alive_timeout_s, n, DEFAULT_KEEP_ALIVE_TIMEOUT, false) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_int("MaxKeepAliveTimeout_s", &server_config.max_keep_alive_timeout_s, n, DEFAULT_MAX_KEEP_ALIVE_TIMEOUT, false) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_int("HangSessionTimeout_s", &server_config.hang_session_timeout_s, n, DEFAULT_HANG_SESSION_TIMEOUT, false) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    



    // //get CgiDirectories
    // if (!(tmp = jtoc_node_get_by_path(n, "CgiDirectories"))
    //     || tmp->type != array) {
    //     LOG_ERROR("Failed to read json ServerInstances CgiDirectories");
    //     return (FUNCTION_FAILURE);
    // }
    // if (ws_jtoc_get_servers_config(server_config.cgi_directory_paths, tmp) == FUNCTION_FAILURE) {

    //     return (FUNCTION_FAILURE);
    // }
    return (FUNCTION_SUCCESS);
}

int ws_jtoc_get_port_servers_configs(std::unordered_map<int, PortServersConfig>& port_servers_configs,
                                t_jnode	*n)
{
    t_jnode         *tmp;
    PortServersConfig port_servers_config_item;
    ServerConfig server_config;

    std::vector<SharedPtr <ServerConfig >> server_configs;
        
    tmp = n->down;
    while(tmp) {
        if (tmp->type != object) {
            LOG_ERROR("Failed to read json array ServerInstances");
            return (FUNCTION_FAILURE);
        }
        if (ws_jtoc_get_server_config(server_config, tmp) == FUNCTION_FAILURE) {
            return (FUNCTION_FAILURE);
        }
        if (port_servers_configs.find(server_config.port) != port_servers_configs.end()){
            // server_config.default_name = true;
            //consrtuctor???
            // port_servers_config_item; // = new empty 
            port_servers_config_item = PortServersConfig();
            port_servers_config_item.port = server_config.port;
            port_servers_config_item.server_configs = std::vector<SharedPtr <ServerConfig >>();
            port_servers_config_item.server_configs.push_back(MakeShared(server_config));
            port_servers_configs.insert({server_config.port, port_servers_config_item});
        }
        else{
            port_servers_config_item = port_servers_configs.at(server_config.port);
            // TODO check repeated names by loop in vectors
            port_servers_config_item.server_configs.push_back(MakeShared(server_config));
        }
        port_servers_configs.push_back(MakeShared(server_config));
        tmp = tmp->right;
    }

    return (FUNCTION_SUCCESS);
}

int				ws_jtoc_get_config(Config& config,
                                 const char *json)
{
    t_jnode	*root;
    t_jnode	*tmp;

    if (!(root = jtoc_read(json))) {
        LOG_ERROR("Failed to read json");
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_int("MaxSocketsNumber", &config.max_sockets_number, root, DEFAULT_MAX_SOCKETS, false) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_int("ReadBufferSize", &config.read_buffer_size, root, DEFAULT_READ_BUFFER_SIZE, false) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_int("SessionsKillerDelay_s", &config.sessions_killer_delay_s, root, DEFAULT_SESSION_KILLER_DELAY, false) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_int("CoreTimeout_s", &config.core_timeout_ms, root, DEFAULT_CORE_TIMEOUT, false) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    ///TODO check SessionsKillerDelay_s and CoreTimeout_s

    //get ServerInstances
    if (!(tmp = jtoc_node_get_by_path(root, "ServerInstances"))
        || tmp->type != array) {
        LOG_ERROR("Failed to read json ServerInstances");
        return (FUNCTION_FAILURE);
    }
    return(ws_jtoc_get_port_servers_configs(config.port_servers_configs, tmp));
}