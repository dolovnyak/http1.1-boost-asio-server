#include "parse_config.h"

int ws_jtoc_get_available_methods(std::unordered_set<Http::Method, EnumClassHash>& available_methods,
                       t_jnode	*n)
{
    t_jnode *tmp;
    t_jnode *loop_iter;
    std::string tmp_str;
    Http::Method method;

    loop_iter = n->down;
    while(loop_iter) {
        tmp = loop_iter;
        if (tmp->type != string) {
            LOG_ERROR("Failed to read json AvailableMethods");
            return (FUNCTION_FAILURE);
        }
        tmp_str = jtoc_get_string(tmp);
        method = Http::GetMethod(tmp_str);
        switch (method) { // check with Petr
            case Http::Method::UNKNOWN: {
                LOG_ERROR("Failed to read json AvailableMethods");
                return (FUNCTION_FAILURE);
            }
            default:
                available_methods.insert(method);
                break;
        }
        loop_iter = loop_iter->right;
    }
    return (FUNCTION_SUCCESS);
}

int ws_jtoc_get_locations(std::vector<SharedPtr<Location> >& locations_vector,
                       t_jnode	*n, std::string server_config_root)
{
    t_jnode	*tmp;
    t_jnode	*loop_iter;
    SharedPtr<Location> tmp_location;
    int tmp_int;
    std::string tmp_string;

    loop_iter = n->down;
    if (loop_iter == NULL) {
        LOG_ERROR("Failed to read json Locations");
        return (FUNCTION_FAILURE);
    }
    while(loop_iter) {
        tmp = loop_iter;
        if (tmp->type != object) {
            LOG_ERROR("Failed to read json Locations");
            return (FUNCTION_FAILURE);
        }
        tmp_location = MakeShared(Location());

        if (ws_jtoc_get_string("Location", &tmp_location->location, loop_iter, std::string(), true) == FUNCTION_FAILURE) {
            return (FUNCTION_FAILURE);
        }
        if (tmp_location->location.length() < 1 or tmp_location->location[0] != '/') {
            LOG_ERROR("Failed to read json Location");
            return (FUNCTION_FAILURE);
        }

        if (ws_jtoc_get_string("Root", &tmp_location->root, loop_iter, server_config_root, false) == FUNCTION_FAILURE) {
            return (FUNCTION_FAILURE);
        }
        if (tmp_location->root.back() != '/') {
            tmp_location->root += '/';
        }

        tmp_location->full_path = tmp_location->root + tmp_location->location.substr(1, tmp_location->location.length() - 1);
        
        // Autoindex
        tmp = jtoc_node_get_by_path(loop_iter, "Autoindex");
        tmp_location->autoindex = false;
        if (tmp != NULL) {
            if (tmp->type != integer) {
                LOG_ERROR("Failed to read json Autoindex");
                return (FUNCTION_FAILURE);  
            }
            tmp_int = jtoc_get_int(tmp);
            if (tmp_int == 0) {
                tmp_location->autoindex = false;
            }
            else if (tmp_int == 1) {
                tmp_location->autoindex = true;
            }
            else {
                LOG_ERROR("Failed to read json Autoindex");
                return (FUNCTION_FAILURE);  
            }
        }

        //index
        tmp = jtoc_node_get_by_path(loop_iter, "Index");
        tmp_location->index = std::string();
        if (tmp != NULL) {
            if (tmp->type != string) { 
                LOG_ERROR("Failed to read json Index");
                return (FUNCTION_FAILURE);
            }
            tmp_location->index = jtoc_get_string(tmp);
            tmp_location->autoindex = false;
        }
        else if (tmp_location->autoindex == false) {
            tmp_location->index = "index.html";
        }

        //AvailableMethods
        tmp_location->available_methods = std::unordered_set<Http::Method, EnumClassHash>();
        tmp = jtoc_node_get_by_path(loop_iter, "AvailableMethods");
        if (tmp != NULL) {
            if (tmp->type != array) {
                LOG_ERROR("Failed to read json AvailableMethods");
                return (FUNCTION_FAILURE);
            }
            else if (ws_jtoc_get_available_methods(tmp_location->available_methods, tmp) == FUNCTION_FAILURE) {
                return (FUNCTION_FAILURE); 
            }
        }

        tmp_location->redirect = std::string();
        tmp = jtoc_node_get_by_path(loop_iter, "Redirect");
        if (tmp != NULL) {
            if (tmp->type != string) {
                LOG_ERROR("Failed to read json Redirect");
                return (FUNCTION_FAILURE);
            }
            tmp_location->redirect = jtoc_get_string(tmp);
        }
        locations_vector.push_back(tmp_location);
        loop_iter = loop_iter->right;
    }
    return (FUNCTION_SUCCESS);
}