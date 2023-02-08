#include "Config.h"

Location::Location(std::string location, std::optional<std::string> root,
                   bool autoindex, std::optional<std::string> index,
                   std::unordered_set<Http::Method> available_methods, std::optional<std::string> redirect)
        : location(std::move(location)), root(std::move(root)), full_path(),
          autoindex(autoindex), index(std::move(index)),
          available_methods(std::move(available_methods)), redirect(std::move(redirect)) {
/// fill full_path
}

ServerConfig::ServerConfig(std::string name, std::string host, unsigned short port,
                           std::unordered_map<Http::Code, std::string> error_pages, int max_body_size,
                           int max_request_size,
                           int default_keep_alive_timeout_s, int max_keep_alive_timeout_s,
                           const std::vector<std::shared_ptr<Location>>& locations)
        : name(std::move(name)), host(std::move(host)), port(port), error_pages(std::move(error_pages)),
          max_body_size(max_body_size), max_request_size(max_request_size),
          default_keep_alive_timeout_s(default_keep_alive_timeout_s),
          max_keep_alive_timeout_s(max_keep_alive_timeout_s),
          locations(locations) {}

EndpointConfig::EndpointConfig(std::string host, unsigned short port,
                               std::vector<std::shared_ptr<ServerConfig>> servers)
                               : host(std::move(host)), port(port), servers(std::move(servers))
                               {}

std::shared_ptr<ServerConfig> EndpointConfig::GetServerByNameOrDefault(const std::string& name) const {
    for (const auto& server: servers) {
        if (server->name == name) {
            return server;
        }
    }

    return GetDefaultServer();
}

std::shared_ptr<ServerConfig> EndpointConfig::GetDefaultServer() const {
    if (servers.empty()) {
        throw std::logic_error("GetDefaultServerConfig on empty server configs");
    }
    return servers[0];
}

Config::Config(unsigned int max_sessions_number,
               unsigned int sessions_killer_delay_s,
               unsigned int hang_session_timeout_s,
               const std::vector<std::shared_ptr<EndpointConfig>>& endpoint_configs)
        : max_sessions_number(max_sessions_number),
          sessions_killer_delay_s(sessions_killer_delay_s),
          hang_session_timeout_s(hang_session_timeout_s),
          endpoint_configs(endpoint_configs) {}
