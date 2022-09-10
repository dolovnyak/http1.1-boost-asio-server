#pragma once

#include "SharedPtr.h"
#include "Http.h"
#include "utilities.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#define DEFAULT_MAX_SOCKETS 1024
#define DEFAULT_READ_BUFFER_SIZE 2048

#define DEFAULT_SESSION_KILLER_DELAY 2 // 2 seconds
#define DEFAULT_CORE_TIMEOUT 1 // 1 seconds


#define DEFAULT_MAX_BODY_SIZE 100000000 // 100 MB
#define DEFAULT_MAX_HEADER_SIZE 1000000 // 1 MB
#define DEFAULT_MAX_RAW_REQUEST_SIZE 200000000 // 200 MB

#define DEFAULT_KEEP_ALIVE_TIMEOUT 60 // 60 seconds
#define DEFAULT_MAX_KEEP_ALIVE_TIMEOUT 1800 // 30 minutes
#define DEFAULT_HANG_SESSION_TIMEOUT 10 // 10 seconds

struct Location{
    Location(
        std::string location,
        std::string root,
        bool autoindex,
        std::string index,
        std::unordered_set<std::string> available_methods,
        std::string redirect);

    std::string location; /// Tlocation должен всегда начинаться с / (кидать ошибку если это не так) /// обязательное поле

    std::string root; /// root должен всегда заканчиваться на / (автоматически добавлять если это не так)  /// опциональное поле, если нет - то записывать от рута сервера

    std::string full_path;  // root + location (тут будет трабла, что рут заканчивается на /, а локейшн начинается на /)

    bool autoindex;  /// по умолчанию false, если указан как true, но есть index то false

    std::string index;  /// если не указан и не указан autoindex то index = index.html

    std::unordered_set<Http::Method, EnumClassHash> available_methods; /// если не указано, то недоступен ни один метод (на всякий, проверить)

    std::string redirect;  /// опциональное поле, хз как работает если задан index или autoindex (на всякий, проверить)

    bool is_default; /// если он один слушает свой порт, то он дефолтный.
};

struct ServerConfig {
    ServerConfig(int port,
                const std::string& name,
                const std::string& root_path,
                const std::unordered_set<std::string>& cgi_file_extensions,
                const std::string& default_file_name,
                int max_body_size,
                int max_header_size,
                int max_raw_request_size,
                int default_keep_alive_timeout_s,
                int max_keep_alive_timeout_s,
                int hang_session_timeout_s);

    int port; /// обязательное поле

    std::string name; /// обязательное поле (проверять, что несколько

    std::string root; /// root должен всегда заканчиваться на / (автоматически добавлять если это не так)  /// обязательное поле

    std::unordered_map <int, std::string> error_pages; /// опциональное поле, они задаются в абсолютном пути (по умолчанию пустое)

    std::unordered_set<std::string> cgi_file_extensions; /// опциональное поле (по умолчанию пустое)

    int default_keep_alive_timeout_s; /// опциональное брать дефолтное значение

    int max_keep_alive_timeout_s; /// опциональное брать дефолтное значение

    int hang_session_timeout_s; /// опциональное брать дефолтное значение

    std::vector<Location> locations;  /// TODO maybe del shared ptr
};

struct PortServersConfig {
    SharedPtr<ServerConfig> GetByNameOrDefault(const std::string& name) const;

    std::vector<SharedPtr <ServerConfig > > server_configs;

    int port;
};

struct Config {
    bool Load(const char* path);

    int max_sockets_number; /// опциональное поле, по умолчанию 1024

    int read_buffer_size; /// опциональное поле, по умолчанию 2048

    int sessions_killer_delay_s; /// опциональное поле, по умолчанию 2

    int core_timeout_ms;  /// this value conflict with sessions_killer_delay_s, so this value should be <= sessions_killer_delay_s (set exception on ths when load config)

    std::unordered_map<int, PortServersConfig> port_servers_configs;
};
