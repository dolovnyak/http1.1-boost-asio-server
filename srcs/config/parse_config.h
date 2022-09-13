#include "Logging.h"
#include "Config.h"

#include "libjtoc.h"
#include <utility>

int				ws_jtoc_get_config(Config& config,
                                 const char *json);

int ws_jtoc_get_port_servers_configs(std::unordered_map<int, SharedPtr<PortServersConfig>>& port_servers_configs_map,
                                t_jnode	*n);

int ws_jtoc_get_server_config(ServerConfig& server_config,
                       t_jnode	*n);

int ws_jtoc_get_error_pages(std::unordered_map <int, std::string>& error_pages,
                       t_jnode	*n);

int ws_jtoc_get_cgi_file_extensions(std::unordered_set<std::string>& cgi_file_extensions,
                       t_jnode	*n);

int ws_jtoc_get_locations(std::vector<SharedPtr<Location> >& locations_vector,
                       t_jnode	*n, std::string server_config_root);
                
int ws_jtoc_get_available_methods(std::unordered_set<Http::Method, EnumClassHash>& available_methods,
                       t_jnode	*n);

int ws_jtoc_get_int(const char *key, int *value, t_jnode *n,
                        int default_int, bool mandatory_flag);

int ws_jtoc_get_string(const char *key, std::string *value, t_jnode *n,
                        std::string default_string, bool mandatory_flag);

bool is_number(const std::string& s);