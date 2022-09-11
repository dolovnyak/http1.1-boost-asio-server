#include "parse_config.h"
#include "Config.h"
#include "libjtoc.h"
#include "Logging.h"
#include <sstream>


// TODO check if arrays are not epty while using while
// TODO update logs

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int             ws_jtoc_get_int(const char *key, int *value, t_jnode *n, int default_int, bool mandatory_flag)
{
    t_jnode	*tmp;
    std::string error;

    error += "Failed to read json ";
    error += key;
    tmp = jtoc_node_get_by_path(n, key);
    if (tmp != NULL){
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

int ws_jtoc_get_locations(std::vector<Location>& locations_vector,
                       t_jnode	*n)
{
    t_jnode	*tmp;
    Location tmp_location;

    tmp = n->down;
    while(tmp) {
        if (tmp->type != object) {
            LOG_ERROR("Failed to read json Locations");
            return (FUNCTION_FAILURE);
        }
        tmp_location = Location();

        // get Location
        if (!(tmp = jtoc_node_get_by_path(tmp, "Location"))
            || tmp->type != string) {
            LOG_ERROR("Failed to read json ServerInstances->Locations->Location");
            return (FUNCTION_FAILURE);
        }
        tmp_location.location = jtoc_get_string(tmp);
        // TODO add checking '/'

        if (!(tmp = jtoc_node_get_by_path(tmp, "Root"))
            || tmp->type != string) {
                LOG_ERROR("Failed to read json ServerInstances->Location->Root");
                return (FUNCTION_FAILURE);
        }
        tmp_location.root = jtoc_get_string(tmp);
        // TODO add checking '/'

        //TODO full_path

        // Autoindex
        tmp = jtoc_node_get_by_path(n, "Autoindex");
        tmp_location.autoindex = false;
        if (tmp != NULL) {
            if (tmp->type != integer) { //!!!!!! check if there if bool type
                LOG_ERROR("Failed to read json Autoindex");
                return (FUNCTION_FAILURE);
            }
            tmp_location.autoindex = jtoc_get_int(tmp);
        }
        tmp = tmp->right;

        //index
        tmp = jtoc_node_get_by_path(n, "Index");
        tmp_location.index = std::string();
        if (tmp != NULL) {
            if (tmp->type != string) { //!!!!!! check if there if bool type
                LOG_ERROR("Failed to read json Index");
                return (FUNCTION_FAILURE);
            }
            tmp_location.index = jtoc_get_string(tmp);
            tmp_location.autoindex = false;
        }
        else if (tmp_location.autoindex == false) {
            tmp_location.index = "index.html";
        }
        // TODO check my logic by config.h

        // AvailableMethods std::unordered_set<Http::Method, EnumClassHash> available_methods; /// если не указано, то недоступен ни один метод (на всякий, проверить)

       //  Redirect std::string redirect;



        tmp = tmp->right;


    }




    return (FUNCTION_SUCCESS);
}

int ws_jtoc_get_cgi_file_extensions(std::unordered_set<std::string>& cgi_file_extensions,
                       t_jnode	*n)
{
    t_jnode	*tmp;

    tmp = n->down;
    while(tmp) {
        if (tmp->type != string) {
            LOG_ERROR("Failed to read json CgiExtensions ");
            return (FUNCTION_FAILURE);
        }
        cgi_file_extensions.insert(jtoc_get_string(tmp));
        tmp = tmp->right;
    }
    return (FUNCTION_SUCCESS);
}

int ws_jtoc_get_error_pages(std::unordered_map <int, std::string>& error_pages,
                       t_jnode	*n)
{
    t_jnode	*tmp;
    int tmp_error_code;

    tmp = n->down;
    while(tmp) {
        if (tmp->type != string) {
            LOG_ERROR("Failed to read json DefaultErrorPages ", tmp->name);
            return (FUNCTION_FAILURE);
        }
        if (is_number(tmp->name) == false)
        {
            LOG_ERROR("Failed to read json DefaultErrorPages ", tmp->name);
            return (FUNCTION_FAILURE);    
        }
        try {
            tmp_error_code = ParsePositiveInt(tmp->name, 10);
        }
        catch(...) {
            LOG_ERROR("Failed to read json DefaultErrorPages ", tmp->name);
            return (FUNCTION_FAILURE);
        }
        LOG_INFO(tmp_error_code);
        error_pages.insert({tmp_error_code, jtoc_get_string(tmp)});
        tmp = tmp->right;
    }
    return (FUNCTION_SUCCESS);
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

    // get root
    if (!(tmp = jtoc_node_get_by_path(n, "Root"))
        || tmp->type != string) {
        LOG_ERROR("Failed to read json ServerInstances Root");
        return (FUNCTION_FAILURE);
    }
    /// TODO add '/' 
    server_config.root = jtoc_get_string(tmp);

    //get DefaultErrorPages
    tmp = jtoc_node_get_by_path(n, "DefaultErrorPages");
    server_config.error_pages = std::unordered_map <int, std::string>();
    if (tmp != NULL){
        if (tmp->type != object) {
            LOG_ERROR("Failed to read json DefaultErrorPages");
            return (FUNCTION_FAILURE);
        }
        else if (ws_jtoc_get_error_pages(server_config.error_pages, tmp) == FUNCTION_FAILURE){
            return (FUNCTION_FAILURE);
        }
    }

    //get CgiExtensions
    tmp = jtoc_node_get_by_path(n, "CgiExtensions");
    server_config.cgi_file_extensions = std::unordered_set<std::string>();
    if (tmp != NULL){
        if (tmp->type != array) {
            LOG_ERROR("Failed to read json CgiExtensions");
            return (FUNCTION_FAILURE);
        }
        else if (ws_jtoc_get_cgi_file_extensions(server_config.cgi_file_extensions, tmp) == FUNCTION_FAILURE){
            return (FUNCTION_FAILURE);
        }
    }

    //get Locations
    if (!(tmp = jtoc_node_get_by_path(n, "Locations"))
        || tmp->type != array) {
        LOG_ERROR("Failed to read json Locations");
        return (FUNCTION_FAILURE);
    }
    if (ws_jtoc_get_locations(server_config.locations, tmp) == FUNCTION_FAILURE){
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

/// TODO check if order stays during reading json
/// TODO GetByNameOrDefault
int ws_jtoc_get_port_servers_configs(std::unordered_map<int, SharedPtr<PortServersConfig>>& port_servers_configs_map,
                                t_jnode	*n)
{
    t_jnode                         *tmp;
    SharedPtr<PortServersConfig>    tmp_port_servers_config_item;
    ServerConfig                    tmp_server_config_item;

    std::vector<SharedPtr <ServerConfig >> server_config_vect;
        
    tmp = n->down;
    while(tmp) {
        if (tmp->type != object) {
            LOG_ERROR("Failed to read json array ServerInstances");
            return (FUNCTION_FAILURE);
        }
        if (ws_jtoc_get_server_config(tmp_server_config_item, tmp) == FUNCTION_FAILURE) {
            return (FUNCTION_FAILURE);
        }
        if (port_servers_configs_map.find(tmp_server_config_item.port) != port_servers_configs_map.end()){
            //port exist
            //extract existing value
            tmp_port_servers_config_item = port_servers_configs_map.at(tmp_server_config_item.port);
            // push_back in value server_configs
            tmp_port_servers_config_item->server_configs.push_back(MakeShared(tmp_server_config_item));
            ///TODO check names repeating
        }
        else{
            // port_servers_config_item create new empty
            //
            tmp_port_servers_config_item = MakeShared(PortServersConfig());
            tmp_port_servers_config_item->port = tmp_server_config_item.port;
            tmp_port_servers_config_item->server_configs = std::vector<SharedPtr <ServerConfig >>();
            tmp_port_servers_config_item->server_configs.push_back(MakeShared(tmp_server_config_item));
            port_servers_configs_map.insert({tmp_server_config_item.port, tmp_port_servers_config_item});
        }
        LOG_INFO("Port number port name: ", tmp_server_config_item.port, tmp_server_config_item.name);
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

    if (ws_jtoc_get_int("HangSessionTimeout_s", &config.hang_session_timeout_s, root, DEFAULT_HANG_SESSION_TIMEOUT, false) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_int("DefaultKeepAliveTimeout_s", &config.default_keep_alive_timeout_s, root, DEFAULT_KEEP_ALIVE_TIMEOUT, false) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_int("MaxKeepAliveTimeout_s", &config.max_keep_alive_timeout_s, root, DEFAULT_MAX_KEEP_ALIVE_TIMEOUT, false) == FUNCTION_FAILURE){
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