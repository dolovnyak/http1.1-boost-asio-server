#include "Config.h"

#include <utility>
#include <stdio.h>



#include <iostream>
#include "libjtoc.h"
#include "logging.h"

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
    std::vector<ServerConfig> mock_1_server_config() {
        std::vector<ServerConfig> configs;

        ServerConfig config("Kabun", 2222, "examples/TODO", {}, 10000);
        configs.push_back(config);
        return configs;
    }

/// TODO delete
    std::vector<ServerConfig> mock_2_server_configs() {
        std::vector<ServerConfig> configs;

        ServerConfig config1("HelloWorld", 1337, "examples/hello_world/", {}, 1000);
        configs.push_back(config1);

        ServerConfig config2("CgiChecker", 1488, "examples/cgi_checker/",
                             {"examples/cgi_checker/cgi1/", "examples/cgi_checker/cgi2"}, 1000);

        configs.push_back(config2);
        return configs;
    }
}


bool Config::Load(const char* path) {
    /// TODO delete
    if (std::string(path) == "1") {
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

        ws_jtoc_setup(config, "conf/default_config.json");
        std::cout << config.threads_number << std::endl;
        std::cout << config.max_sockets_number << std::endl;
        std::cout << config.timeout << std::endl;
        LOG_INFO("name: ", config.servers_configs[0].name);
        std::cout << config.servers_configs[0].port << std::endl;
        LOG_INFO("root_path: ", config.servers_configs[0].root_path);
        std::cout << config.servers_configs[0].cgi_directory_paths[0] << std::endl;
        std::cout << config.servers_configs[0].cgi_directory_paths[1] << std::endl;
        std::cout << config.servers_configs[0].max_connection_number<< std::endl;
        std::cout << config.servers_configs[1].name << std::endl;
        std::cout << config.servers_configs[1].port << std::endl;
        std::cout << config.servers_configs[1].root_path << std::endl;
        std::cout << config.servers_configs[1].cgi_directory_paths[0] << std::endl;
        std::cout << config.servers_configs[1].cgi_directory_paths[1] << std::endl;
        std::cout << config.servers_configs[1].max_connection_number<< std::endl;
    }
    return false;
}

/*
uint32_t Config::GetThreadsNumber() const {
    return _threads_number;
}

/// TODO make max events number and max sockets number
uint32_t Config::GetMaxEventsNumber() const {
    return _max_sockets_number;
}

const std::vector<ServerConfig>& Config::GetServersConfigs() const {
    return _servers_configs;
}

int32_t Config::GetTimeout() const {
    return _timeout;
}
*/

ServerConfig::ServerConfig(std::string name, uint16_t port, std::string root_path,
                           std::vector<std::string> cgi_directory_paths,
                           int32_t max_connection_number)
        : name(std::move(name)), port(port), root_path(std::move(root_path)),
          cgi_directory_paths(std::move(cgi_directory_paths)),
          max_connection_number(max_connection_number) {}
