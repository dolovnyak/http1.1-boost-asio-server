#pragma once

#include "SharedPtr.h"
#include "Http.h"
#include "utilities.h"


#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#define WEBSERVER_NAME "Webserver-42"

#define DEFAULT_MAX_SOCKETS 1024
#define DEFAULT_READ_BUFFER_SIZE 4096

#define DEFAULT_SESSION_KILLER_DELAY 2 // 2 seconds
#define DEFAULT_CORE_TIMEOUT 1 // 1 seconds

#define DEFAULT_MAX_BODY_SIZE 100000000 // 100 MB
#define DEFAULT_MAX_HEADER_SIZE 1000000 // 1 MB
#define DEFAULT_MAX_REQUEST_SIZE 200000000 // 200 MB

#define DEFAULT_KEEP_ALIVE_TIMEOUT 60 // 60 seconds
#define DEFAULT_MAX_KEEP_ALIVE_TIMEOUT 1800 // 30 minutes
#define DEFAULT_HANG_SESSION_TIMEOUT 10 // 10 seconds

struct PortServersConfig;
typedef std::unordered_map<int, SharedPtr<PortServersConfig> >::iterator PortServersIt;

struct Location{
    Location(
        std::string location,
        std::string root,
        bool autoindex,
        std::string index,
        std::unordered_set<std::string> available_methods,
        std::string redirect);
    
    Location() {};

    std::string location; /// location должен всегда начинаться с / (кидать ошибку если это не так) /// обязательное поле

    std::string root; /// root должен всегда заканчиваться на / (автоматически добавлять если это не так)  /// опциональное поле, если нет - то записывать от рута сервера

    std::string full_path;  // root + location (тут будет трабла, что рут заканчивается на /, а локейшн начинается на /)

    bool autoindex;  /// по умолчанию false, если указан как true, но есть index то false

    std::string index;  /// если не указан и не указан autoindex то index = index.html

    std::unordered_set<Http::Method, EnumClassHash> available_methods; /// если не указано, то недоступен ни один метод (на всякий, проверить)

    std::string redirect;  /// опциональное поле, хз как работает если задан index или autoindex (на всякий, проверить)
};

struct ServerConfig {
    ServerConfig(int port,
                const std::string& name,
                const std::string& root,
                std::unordered_map <int, std::string> error_pages,
                const std::unordered_set<std::string>& cgi_file_extensions,
                std::vector<SharedPtr<Location> > locations  /// TODO maybe del shared ptr
                );

    ServerConfig() {} // TODO add defaut value

    int port; /// обязательное поле

    std::string name; /// обязательное поле (проверять, что несколько

    std::string root; /// root должен всегда заканчиваться на / (автоматически добавлять если это не так)  /// обязательное поле

    std::unordered_map <int, std::string> error_pages; /// опциональное поле, они задаются в абсолютном пути (по умолчанию пустое)

    std::unordered_set<std::string> cgi_file_extensions; /// опциональное поле (по умолчанию пустое)

    int max_body_size; /// опциональное поле, если не указано то берется из дефолта

    int max_request_size; /// опциональное поле, если не указано то берется из дефолта

    std::vector<SharedPtr<Location> > locations; 
};

struct PortServersConfig {
    
    SharedPtr<ServerConfig> GetByNameOrDefault(const std::string& name) const;

    SharedPtr<ServerConfig> GetDefault() const; // что за const?

    std::vector<SharedPtr <ServerConfig > > server_configs; /// first server is default

    int port;
};

struct Config {
    bool Load(const char* path);

    int max_sockets_number; /// опциональное поле, по умолчанию 1024

    int read_buffer_size; /// опциональное поле, по умолчанию 2048

    int sessions_killer_delay_s; /// опциональное поле, по умолчанию 2

    int core_timeout_ms;  /// this value conflict with sessions_killer_delay_s, so this value should be <= sessions_killer_delay_s (set exception on ths when load config)

    int hang_session_timeout_s; /// опциональное брать дефолтное значение

    int default_keep_alive_timeout_s; /// опциональное брать дефолтное значение

    int max_keep_alive_timeout_s; /// опциональное брать дефолтное значение

    std::unordered_map<int, SharedPtr<PortServersConfig> > port_servers_configs;
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