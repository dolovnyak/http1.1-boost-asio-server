#include "parse_config.h"

int ws_jtoc_get_port_servers_configs(std::unordered_map<int, SharedPtr<PortServersConfig>>& port_servers_configs_map,
                                t_jnode	*n)
{
    t_jnode                                 *tmp;
    t_jnode                                 *loop_iter;
    SharedPtr<PortServersConfig>            tmp_port_servers_config_item;
    ServerConfig                            tmp_server_config_item;
    std::vector<SharedPtr <ServerConfig > > tmp_server_config_vect;

    loop_iter = n->down;
    if (loop_iter == NULL) {
        LOG_ERROR("Failed to read json ServerInstances");
        return (FUNCTION_FAILURE);
    }
    while(loop_iter) {
        tmp = loop_iter;
        if (tmp->type != object) {
            LOG_ERROR("Failed to read json ServerInstances");
            return (FUNCTION_FAILURE);
        }
        if (ws_jtoc_get_server_config(tmp_server_config_item, tmp) == FUNCTION_FAILURE) {
            return (FUNCTION_FAILURE);
        }
        if (port_servers_configs_map.find(tmp_server_config_item.port) != port_servers_configs_map.end()){
            tmp_port_servers_config_item = port_servers_configs_map.at(tmp_server_config_item.port);
            tmp_server_config_vect = tmp_port_servers_config_item->server_configs;
            for (int i = 0; i < (int)tmp_server_config_vect.size(); i++) {
                if (tmp_server_config_vect[i]->name == tmp_server_config_item.name) {
                    LOG_ERROR("Failed to read json ServerInstances: duplication in name ", tmp_server_config_item.name);
                    return (FUNCTION_FAILURE);
                }
            }
            tmp_port_servers_config_item->server_configs.push_back(MakeShared(tmp_server_config_item));
        }
        else{
            tmp_port_servers_config_item = MakeShared(PortServersConfig());
            tmp_port_servers_config_item->port = tmp_server_config_item.port;
            tmp_port_servers_config_item->server_configs = std::vector<SharedPtr <ServerConfig >>();
            tmp_port_servers_config_item->server_configs.push_back(MakeShared(tmp_server_config_item));
            port_servers_configs_map.insert({tmp_server_config_item.port, tmp_port_servers_config_item});
        }
        loop_iter = loop_iter->right;
    }

    return (FUNCTION_SUCCESS);
}