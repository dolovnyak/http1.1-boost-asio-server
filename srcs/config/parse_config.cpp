#include "parse_config.h"
#include "Config.h"
#include "libjtoc.h"
#include "Logging.h"
#include <sstream>


// TODO check if arrays are not empty while using while_loop
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

int             ws_jtoc_get_string(const char *key, std::string *value, t_jnode *n, std::string default_string, bool mandatory_flag)
{
    t_jnode	*tmp;
    std::string error;

    error += "Failed to read json ";
    error += key;

    tmp = jtoc_node_get_by_path(n, key);
    if (tmp != NULL)
    {
        if (tmp->type != string) {
            LOG_ERROR(error);
            return (FUNCTION_FAILURE);
        }
        *value = jtoc_get_string(tmp);
        if (mandatory_flag == true) {
            if (value->length() < 1) {
                LOG_ERROR(error);
                return (FUNCTION_FAILURE);
            }
        }
        return (FUNCTION_SUCCESS);
    }
    else if (mandatory_flag == true){
        LOG_ERROR(error);
        return (FUNCTION_FAILURE);
    }
    *value = default_string;
    return (FUNCTION_SUCCESS);
}
/// TODO check if order stays during reading json

int				ws_jtoc_get_config(Config& config,
                                 const char *json)
{
    t_jnode	*root;
    t_jnode	*tmp;

    if (!(root = jtoc_read(json))) {
        LOG_ERROR("Failed to read json");
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_int("MaxSocketsNumber", &config.max_sessions_number, root, DEFAULT_MAX_SOCKETS, false) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_int("ReadBufferSize", &config.read_buffer_size, root, DEFAULT_READ_BUFFER_SIZE, false) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_int("SessionsKillerDelay_s", &config.sessions_killer_delay_s, root, DEFAULT_SESSION_KILLER_DELAY, false) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_int("CoreTimeout_ms", &config.core_timeout_ms, root, DEFAULT_CORE_TIMEOUT, false) == FUNCTION_FAILURE){
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

    if (config.core_timeout_ms > config.sessions_killer_delay_s * 1000) {
        LOG_ERROR("Failed to read json: CoreTimeout_ms more than SessionsKillerDelay_s * 1000");
        return (FUNCTION_FAILURE);
    }

    //get ServerInstances
    if (!(tmp = jtoc_node_get_by_path(root, "ServerInstances"))
        || tmp->type != array) {
        LOG_ERROR("Failed to read json ServerInstances");
        return (FUNCTION_FAILURE);
    }
    return(ws_jtoc_get_port_servers_configs(config.port_servers_configs, tmp));
}