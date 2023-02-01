#pragma once

#include "Http.h"
#include "utilities.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#define WEBSERVER_NAME "Webserver-42"

#define DEFAULT_MAX_SESSIONS_NUMBER 1024
#define DEFAULT_READ_BUFFER_SIZE 4096

#define DEFAULT_SESSION_KILLER_DELAY 2 // 2 seconds
#define DEFAULT_CORE_TIMEOUT 1 // 1 seconds

#define DEFAULT_HOST

#define DEFAULT_MAX_BODY_SIZE 100000000 // 100 MB
#define DEFAULT_MAX_REQUEST_SIZE 200000000 // 200 MB

#define DEFAULT_KEEP_ALIVE_TIMEOUT 60 // 60 seconds
#define DEFAULT_MAX_KEEP_ALIVE_TIMEOUT 1800 // 30 minutes
#define DEFAULT_HANG_SESSION_TIMEOUT 10 // 10 seconds

struct Location {
    Location(const std::string& location, const std::string& root,
             bool autoindex, const std::string& index,
             const std::vector<std::string>& available_methods,
             const std::string& redirect);

    const std::string location; /// location должен всегда начинаться с / (кидать ошибку если это не так) /// обязательное поле

    const std::string root; /// root должен всегда заканчиваться на / (автоматически добавлять если это не так)  /// опциональное поле, если нет - то записывать от рута сервера

    const std::string full_path;  /// root + location (тут будет трабла, что рут заканчивается на /, а локейшн начинается на /)

    const bool autoindex;  /// по умолчанию false, если указан как true, но есть index то false

    const std::string index;  /// если не указан и не указан autoindex то index = index.html

    const std::unordered_set<Http::Method> available_methods; /// если не указано, то недоступен ни один метод

    const std::string redirect;  /// опциональное поле, хз как работает если задан index или autoindex (на всякий, проверить)
};


struct ServerConfig {
    ServerConfig(const std::string& name, const std::string& host, int port,
                 const std::unordered_map<unsigned int, std::string>& error_pages, int max_body_size,
                 int max_request_size,
                 int default_keep_alive_timeout_s, int max_keep_alive_timeout_s,
                 const std::vector<std::shared_ptr<Location>>& locations)
            : name(name), host(host), port(port), error_pages(error_pages),
              max_body_size(max_body_size), max_request_size(max_request_size),
              default_keep_alive_timeout_s(default_keep_alive_timeout_s),
              max_keep_alive_timeout_s(max_keep_alive_timeout_s),
              locations(locations) {}

    std::string name;

    const std::string host;

    const int port;

    std::unordered_map<unsigned int, std::string> error_pages; /// опциональное поле, они задаются в абсолютном пути (по умолчанию пустое)

    int max_body_size; /// опциональное поле, если не указано то берется из дефолта

    int max_request_size; /// опциональное поле, если не указано то берется из дефолта

    int default_keep_alive_timeout_s; /// опциональное брать дефолтное значение

    int max_keep_alive_timeout_s; /// опциональное брать дефолтное значение

    std::vector<std::shared_ptr<Location>> locations;
};

class Config {
public:
    Config(unsigned int max_sessions_number,
           unsigned int read_buffer_size,
           unsigned int sessions_killer_delay_s,
           unsigned int hang_session_timeout_s,
           const std::vector<std::shared_ptr<ServerConfig>>& server_configs)
            : max_sessions_number(max_sessions_number),
              read_buffer_size(read_buffer_size),
              sessions_killer_delay_s(sessions_killer_delay_s),
              hang_session_timeout_s(hang_session_timeout_s),
              server_configs(server_configs) {}

    [[nodiscard]] std::shared_ptr<ServerConfig> GetServerConfigByNameOrDefault(const std::string& name) const {
        for (const auto& server_config: server_configs) {
            if (server_config->name == name) {
                return server_config;
            }
        }

        return GetDefaultServerConfig();
    }

    [[nodiscard]] std::shared_ptr<ServerConfig> GetDefaultServerConfig() const {
        if (server_configs.empty()) {
            throw std::runtime_error("GetDefaultServerConfig on empty server configs");
        }
        return server_configs[0];
    }

    const unsigned int max_sessions_number;

    const unsigned int read_buffer_size;

    const unsigned int sessions_killer_delay_s;

    const unsigned int hang_session_timeout_s;

    const std::vector<std::shared_ptr<ServerConfig>> server_configs; /// the first one is default
};

/// TODO check if order stays during reading json
/// TODO check availiable methods defaults
/// TODO check redirect behaviare
/// error not Allow если к недопустимому методу 
/// name для location обязательный?
/// проверить root and location in subject

//  NOTICES:
//
//все интовые значения должны быть > 0
//
//парсер читает все строчки, и при повторяющихся ключей берет первое значение  
//
//problem by jtocs. Doen't see comments and empty line, exempl: 
//     "Name": ""
//    //"root": "ssss/"