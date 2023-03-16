#include "Config.h"

#include <stdexcept>

ExtensionInterceptor::ExtensionInterceptor(std::string extension, std::string cgi_path,
                                           std::unordered_set<Http::Method> available_methods)
        : extension(std::move(extension)), cgi_path(std::move(cgi_path)),
          on_methods(std::move(available_methods)) {}

Location::Location(std::string location,
                   std::string root,
                   std::optional<std::string> index,
                   std::optional<HttpReturn> http_return,
                   bool autoindex,
                   std::unordered_set<Http::Method> available_methods,
                   unsigned int max_body_size)
        : location(std::move(location)), root(std::move(root)),
          index(std::move(index)), http_return(std::move(http_return)),
          autoindex(autoindex), available_methods(std::move(available_methods)),
          max_body_size(max_body_size) {
    int forbidden_intersected_fields = 0;
    forbidden_intersected_fields += this->index.has_value();
    forbidden_intersected_fields += this->http_return.has_value();
    forbidden_intersected_fields += this->autoindex;
    if (forbidden_intersected_fields > 1) {
        throw std::runtime_error(
                "Location: \"" + this->location + "\", autoindex, index, return and upload couldn't intersect");
    }

    if (this->location.empty() || this->location[0] != '/'
        || (this->location.size() > 1 && this->location.back() == '/')) {
        throw std::runtime_error("Incorrect location \"" + this->location + "\"");
    }
}

ServerConfig::ServerConfig(std::string name, std::string host, unsigned short port,
                           std::string cgi_uploader_path, std::string cgi_deleter_path,
                           std::unordered_map<Http::Code, std::string> error_pages, int max_body_size,
                           int max_request_size,
                           int default_keep_alive_timeout_s, int max_keep_alive_timeout_s,
                           const std::vector<std::shared_ptr<ExtensionInterceptor>>& extensions_interceptors,
                           const std::vector<std::shared_ptr<Location>>& locations)
        : name(std::move(name)), host(std::move(host)), port(port),
          cgi_uploader_path(std::move(cgi_uploader_path)), cgi_deleter_path(std::move(cgi_deleter_path)),
          error_pages(std::move(error_pages)),
          max_body_size(max_body_size), max_request_size(max_request_size),
          default_keep_alive_timeout_s(default_keep_alive_timeout_s),
          max_keep_alive_timeout_s(max_keep_alive_timeout_s),
          extensions_interceptors(extensions_interceptors),
          locations(locations) {}

EndpointConfig::EndpointConfig(std::string host, unsigned short port,
                               std::vector<std::shared_ptr<ServerConfig>> servers)
        : host(std::move(host)), port(port), servers(std::move(servers)) {}

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

Config::Config(const std::vector<std::shared_ptr<EndpointConfig>>& endpoint_configs)
        : endpoint_configs(endpoint_configs) {}
