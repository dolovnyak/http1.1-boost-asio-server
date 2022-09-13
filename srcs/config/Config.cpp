//#include "Config.h"
#include "parse_config.h"

// namespace {
/// TODO delete when parse will finish

    // std::vector<SharedPtr<ServerConfig> > mock_1_server_config() {
    //     std::vector<SharedPtr<ServerConfig> > configs;

    //     ServerConfig config(2222, "Kabun", "examples/aaaa/", std::unordered_set<std::string>(), "index.html", 10, 300, 10);
    //     configs.push_back(MakeShared(config));
    //     return configs;
    // }

/// TODO delete

    // std::vector<SharedPtr<ServerConfig> > mock_2_server_configs() {
//         std::vector<SharedPtr<ServerConfig> > configs;
//
//         ServerConfig config1(1337, "HelloWorld", "/Users/sbecker/Desktop/projects/webserver-42/", std::unordered_set<std::string>(), "index.html", 60, 300, 20);
//         configs.push_back(MakeShared(config1));

    //     std::unordered_set<std::string> cgi_extensions;
    //     cgi_extensions.insert(".py");
    //     cgi_extensions.insert(".php");
    //     ServerConfig config2(1488, "CgiChecker", "examples/cgi_checker/", cgi_extensions, "index.html", 60, 300, 20);

    //     configs.push_back(MakeShared(config2));
    //     return configs;
    // }
// }

SharedPtr<ServerConfig> PortServersConfig::GetByNameOrDefault(const std::string& name) const {
    SharedPtr <ServerConfig > server_config;

    for (int i = 0; i < (int)server_configs.size(); i++) { 
        server_config = server_configs[i];
        if (server_config->name == name) {
            return (server_config);
        }
    }
    if ((int)server_configs.size() > 0) {
        return server_configs[0];
    }
    return (SharedPtr<ServerConfig>());
}

bool Config::Load(const char* path) {
    Config config;

    if(ws_jtoc_get_config(config, "../../../conf/default_config.json") == FUNCTION_SUCCESS) { // change 2 arg to path
        
        LOG_INFO("START TEST: ");
        LOG_INFO("max_sockets_number: ", config.max_sockets_number);
        LOG_INFO("read_buffer_size: ", config.read_buffer_size);
        LOG_INFO("sessions_killer_delay_s: ", config.sessions_killer_delay_s);
        LOG_INFO("core_timeout_ms: ", config.core_timeout_ms);
        LOG_INFO("hang_session_timeout_s: ", config.hang_session_timeout_s);
        LOG_INFO("default_keep_alive_timeout_s: ", config.default_keep_alive_timeout_s);
        LOG_INFO("max_keep_alive_timeout_s: ", config.max_keep_alive_timeout_s);
        // config.port_servers_configs.begin(); 
        for (std::unordered_map<int, SharedPtr<PortServersConfig> >::iterator it_psc = config.port_servers_configs.begin();
            it_psc != config.port_servers_configs.end(); ++it_psc) {
            LOG_INFO("port in port_servers_configs ", it_psc->first);
            SharedPtr<PortServersConfig> tmp_post_servers_config = it_psc->second;
            //checkeing GetByNameOrDefault
            LOG_ERROR(tmp_post_servers_config->GetByNameOrDefault("name")->name);
            LOG_ERROR(tmp_post_servers_config->GetByNameOrDefault("name3")->name);
            LOG_ERROR(tmp_post_servers_config->GetByNameOrDefault("kabun3")->name);
            LOG_INFO("port in PortServersConfig ", it_psc->second->port);
            for (int i = 0; i < (int)tmp_post_servers_config->server_configs.size(); i++) {
                SharedPtr<ServerConfig > tmp_server_configs = tmp_post_servers_config->server_configs.at(i);
                LOG_INFO("  ServerInstances numb =  ", i);
                LOG_INFO("  port ", tmp_server_configs->port);
                LOG_INFO("  name ", tmp_server_configs->name);
                LOG_INFO("  root ", tmp_server_configs->root);
                LOG_INFO("  max_body_size ", tmp_server_configs->max_body_size);
                LOG_INFO("  max_request_size ", tmp_server_configs->max_request_size);
                LOG_INFO("  error_pages: ");
                for (std::unordered_map <int, std::string>::iterator it_ep = tmp_server_configs->error_pages.begin();
                    it_ep != tmp_server_configs->error_pages.end(); ++it_ep) {
                        LOG_INFO("      ", it_ep->first, " ", it_ep->second);
                }
                LOG_INFO("  cgi_file_extensions: ");
                for (std::unordered_set<std::string>::iterator it_cgi_fe = tmp_server_configs->cgi_file_extensions.begin();
                    it_cgi_fe != tmp_server_configs->cgi_file_extensions.end(); ++it_cgi_fe) {
                        LOG_INFO("      ", *it_cgi_fe);
                }
                LOG_INFO("  locations: ");
                for (int j = 0; j < (int)tmp_server_configs->locations.size(); j++) {
                    LOG_INFO("  locations num ", j);
                    SharedPtr<Location > tmp_location = tmp_server_configs->locations.at(j);
                    LOG_INFO("      location ", tmp_location->location);
                    LOG_INFO("      root ", tmp_location->root);
                    LOG_INFO("      full_path ", tmp_location->full_path);
                    LOG_INFO("      autoindex ", tmp_location->autoindex);
                    LOG_INFO("      index ", tmp_location->index);
                    LOG_INFO("      available_methods: ");
                    for (std::unordered_set<Http::Method, EnumClassHash>::iterator it_am = tmp_location->available_methods.begin();
                    it_am != tmp_location->available_methods.end(); ++it_am) {
                        LOG_INFO("      ", *it_am);
                    }
                    LOG_INFO("      redirect ", tmp_location->redirect);
                }

                
            }
        }
        return false;
    }
    return true;
}

ServerConfig::ServerConfig(int port,
                            const std::string& name,
                            const std::string& root,
                            std::unordered_map <int, std::string> error_pages,
                            const std::unordered_set<std::string>& cgi_file_extensions,
                            std::vector<SharedPtr<Location> > locations
                            )
                    : port(port),
                    name(name),
                    root(root),
                    error_pages(error_pages), 
                    cgi_file_extensions(cgi_file_extensions),
                    locations(locations){}



