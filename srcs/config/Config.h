#pragma once

#include "Http.h"
#include "utilities.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <memory>

#define READ_BUFFER_SIZE 8192

#define DEFAULT_MAX_SESSIONS_NUMBER 1024

#define DEFAULT_SESSION_KILLER_DELAY 2 // 2 seconds
#define DEFAULT_CORE_TIMEOUT 1 // 1 seconds

#define DEFAULT_MAX_BODY_SIZE 10000000000
#define DEFAULT_MAX_REQUEST_SIZE 20000000000

#define DEFAULT_KEEP_ALIVE_TIMEOUT 60 // 60 seconds
#define DEFAULT_MAX_KEEP_ALIVE_TIMEOUT 1800 // 30 minutes
#define DEFAULT_HANG_SESSION_TIMEOUT 10 // 10 seconds

struct HttpReturn {
    Http::Code code;
    std::optional<std::string> redirect;
};

struct ExtensionInterceptor {
    ExtensionInterceptor(std::string extension,
                         std::string cgi_path,
                         std::unordered_set<Http::Method> available_methods);
    const std::string extension;
    const std::string cgi_path;
    const std::unordered_set<Http::Method> on_methods;
};

struct Location {
    Location(std::string location,
             std::string root,
             std::optional<std::string> index,
             std::optional<HttpReturn> http_return,
             bool autoindex,
             std::unordered_set<Http::Method> available_methods,
             unsigned int max_body_size);

    const std::string location;

    const std::string root;

    const std::optional<std::string> index;

    const std::optional<HttpReturn> http_return;

    const bool autoindex;

    const std::unordered_set<Http::Method> available_methods;

    unsigned int max_body_size;
};

struct ServerConfig {
    ServerConfig(std::string name, std::string host, unsigned short port,
                 std::string cgi_uploader_path, std::string cgi_deleter_path,
                 std::unordered_map<Http::Code, std::string> error_pages, int max_body_size,
                 int max_request_size,
                 int default_keep_alive_timeout_s, int max_keep_alive_timeout_s,
                 const std::vector<std::shared_ptr<ExtensionInterceptor>>& extensions_interceptors,
                 const std::vector<std::shared_ptr<Location>>& locations);

    const std::string name;

    const std::string host;

    const unsigned short port;

    const std::string cgi_uploader_path;

    const std::string cgi_deleter_path;

    const std::unordered_map<Http::Code, std::string> error_pages;

    unsigned int max_body_size;

    unsigned int max_request_size;

    unsigned int default_keep_alive_timeout_s;

    unsigned int max_keep_alive_timeout_s;

    std::vector<std::shared_ptr<ExtensionInterceptor>> extensions_interceptors;

    std::vector<std::shared_ptr<Location>> locations;
};

/// servers with the same host:port
class EndpointConfig {
public:
    EndpointConfig(std::string host, unsigned short port, std::vector<std::shared_ptr<ServerConfig>> servers);

    [[nodiscard]] std::shared_ptr<ServerConfig> GetServerByNameOrDefault(const std::string& name) const;

    [[nodiscard]] std::shared_ptr<ServerConfig> GetDefaultServer() const;

    const std::string host;

    const unsigned short port;

    const std::vector<std::shared_ptr<ServerConfig>> servers; /// the first one is default

};

class Config {
public:
    Config(const std::vector<std::shared_ptr<EndpointConfig>>& endpoint_configs);

    const std::vector<std::shared_ptr<EndpointConfig>> endpoint_configs;
};

/// TODO check if order stays during reading json
/// TODO check redirect behaviare
/// error not Allow если к недопустимому методу 
/// name для location обязательный?
/// проверить root and location in subject