#include "parse_config.h"

int ws_jtoc_get_cgi_file_extensions(std::unordered_set<std::string>& cgi_file_extensions,
                       t_jnode	*n)
{
    t_jnode	*tmp;
    t_jnode	*loop_iter;

    loop_iter = n->down;
    while(loop_iter) {
        tmp = loop_iter;
        if (tmp->type != string) {
            LOG_ERROR("Failed to read json CgiExtensions ");
            return (FUNCTION_FAILURE);
        }
        cgi_file_extensions.insert(jtoc_get_string(tmp));
        loop_iter = loop_iter->right;
    }
    return (FUNCTION_SUCCESS);
}

int ws_jtoc_get_error_pages(std::unordered_map <int, std::string>& error_pages,
                       t_jnode	*n)
{
    t_jnode	*tmp;
    t_jnode *loop_iter;
    int tmp_error_code;

    loop_iter = n->down;
    while(loop_iter) {
        tmp = loop_iter;
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
        error_pages.insert({tmp_error_code, jtoc_get_string(tmp)});
        loop_iter = loop_iter->right;
    }
    return (FUNCTION_SUCCESS);
}

int ws_jtoc_get_server_config(ServerConfig& server_config,
                       t_jnode	*n)
{
    t_jnode	*tmp;
    std::vector<std::string> cgi_directory_paths;

    if (ws_jtoc_get_int("Port", &server_config.port, n, -1, true) == FUNCTION_FAILURE) {
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_string("Name", &server_config.name, n, std::string(), true) == FUNCTION_FAILURE) {
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_string("Root", &server_config.root, n, std::string(), true) == FUNCTION_FAILURE) {
        return (FUNCTION_FAILURE);
    }
    if (server_config.root.back() != '/') {
        server_config.root += '/';
    }

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

    if (ws_jtoc_get_int("MaxBodySize", &server_config.max_body_size, n, DEFAULT_MAX_BODY_SIZE, false) == FUNCTION_FAILURE) {
        return (FUNCTION_FAILURE);
    }

    if (ws_jtoc_get_int("MaxRequestSize", &server_config.max_request_size, n, DEFAULT_MAX_REQUEST_SIZE, false) == FUNCTION_FAILURE) {
        return (FUNCTION_FAILURE);
    }

    //get Locations
    if (!(tmp = jtoc_node_get_by_path(n, "Locations"))
        || tmp->type != array) {
        LOG_ERROR("Failed to read json Locations");
        return (FUNCTION_FAILURE);
    }
    else if (ws_jtoc_get_locations(server_config.locations, tmp, server_config.root) == FUNCTION_FAILURE){
        return (FUNCTION_FAILURE);
    }
    return (FUNCTION_SUCCESS);
}