#include "Logging.h"
#include "Config.h"

#include "libjtoc.h"
#include <utility>

int ws_jtoc_get_servers_config(std::vector<std::string>& str,
                               t_jnode	*n);

int ws_jtoc_extract_int(const char *key, int *value, t_jnode *n);


int ws_jtoc_get_servers_config(SharedPtr<ServerConfig>& config,
                               t_jnode	*n);

int ws_jtoc_get_servers_configs(std::vector<SharedPtr<ServerConfig>> & server_configs,
                                t_jnode	*n);

int				ws_jtoc_setup(Config& config,
                                 const char *json);
