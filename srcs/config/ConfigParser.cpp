#include "ConfigParser.h"
#include "Config.h"
#include "Logging.h"

#include <boost/json/src.hpp>
#include <fstream>


namespace {
    boost::json::value file_to_json(char const* filename) {
        std::ifstream file(filename);
        if (!file.is_open()) { throw std::runtime_error("Can't open json file"); }

        boost::json::stream_parser p;
        boost::json::error_code ec;
        while (!file.eof()) {
            char buf[JSON_READ_BUFFER_SIZE];
            file.read(buf, sizeof(buf));
            size_t extracted_size = file.gcount();
            p.write(buf, extracted_size, ec);
        }

        if (ec) { throw std::runtime_error("Can't parse json file"); }
        p.finish(ec);
        if (ec) { throw std::runtime_error("Can't parse json file"); }
        return p.release();
    }

    template<class T>
    T extract(const boost::json::object& obj, boost::json::string_view key, const T& default_value) {
        try {
            auto it = obj.find(key);
            if (it == obj.end()) {
                return default_value;
            }
            return boost::json::value_to<T>(obj.at(key));
        }
        catch (const boost::system::system_error& e) {
            throw std::runtime_error("Error in field \"" + std::string(key)+ "\"\n");
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Error in field \"" + std::string(key)+ "\"\n" + std::string(e.what()));
        }
    }

    template<class T>
    T extract(const boost::json::object& obj, boost::json::string_view key) {
        try {
            return boost::json::value_to<T>(obj.at(key));
        }
        catch (const boost::system::system_error& e) {
            throw std::runtime_error("Error in field \"" + std::string(key)+ "\"\n");
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Error in field \"" + std::string(key)+ "\"\n" + std::string(e.what()));
        }
    }

    template<class Container>
    Container sequence_container_extract(const boost::json::object& obj, boost::json::string_view key) {
        try {
            Container res;
            const boost::json::array& arr = obj.at(key).as_array();
            for (auto& arr_elem: arr) {
                res.emplace_back(boost::json::value_to<typename Container::value_type>(arr_elem));
            }
            return res;
        }
        catch (const boost::system::system_error& e) {
            throw std::runtime_error("Error in field \"" + std::string(key)+ "\"\n");
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Error in field \"" + std::string(key)+ "\"\n" + std::string(e.what()));
        }
    }

    struct endpoint_hash {
        std::size_t operator()(const std::pair<std::string, unsigned short>& pair) const {
            return std::hash<std::string>()(pair.first + std::to_string(pair.second));
        }
    };

    std::vector<std::shared_ptr<EndpointConfig>>
    PackToEndpoints(std::vector<std::shared_ptr<ServerConfig>>&& server_configs) {
        std::unordered_map<std::pair<std::string, unsigned short>, std::vector<std::shared_ptr<ServerConfig>>, endpoint_hash> servers_by_endpoint;
        for (auto& server: server_configs) {
            servers_by_endpoint[{server->host, server->port}].emplace_back(std::move(server));
        }
        std::vector<std::shared_ptr<EndpointConfig>> endpoints;
        for (auto& server_by_endpoint: servers_by_endpoint) {
            std::string host = server_by_endpoint.first.first;
            unsigned short port = server_by_endpoint.first.second;
            std::vector<std::shared_ptr<ServerConfig>>& servers = server_by_endpoint.second;
            endpoints.emplace_back(std::make_shared<EndpointConfig>(host, port, std::move(servers)));
        }
        return endpoints;
    }

    std::unordered_set<Http::Method>
    ToHandledMethods(std::vector<std::string>&& raw_methods) {
        std::unordered_set<Http::Method> methods;
        for (auto& raw_method: raw_methods) {
            Http::Method method = Http::ToHttpMethod(raw_method);
            methods.emplace(method);
        }
        return methods;
    }
}

std::shared_ptr<Location>
tag_invoke(const boost::json::value_to_tag<std::shared_ptr<Location>>&, boost::json::value const& json) {
    const boost::json::object& obj = json.as_object();

    return std::make_shared<Location>(
            extract<std::string>(obj, "Location"),
            extract<std::string>(obj, "Root"),
            extract<bool>(obj, "Autoindex"),
            extract<std::string>(obj, "Index"),
            ToHandledMethods(sequence_container_extract<std::vector<std::string>>(obj, "AvailableMethods")),
            extract<std::string>(obj, "Redirect"));
}

std::pair<unsigned int, std::string>
tag_invoke(const boost::json::value_to_tag<std::pair<unsigned int, std::string>>&,
           boost::json::value const& json) {
    const boost::json::object& obj = json.as_object();

    return {extract<unsigned int>(obj, "Code"),
            extract<std::string>(obj, "Path")};
}

std::shared_ptr<ServerConfig>
tag_invoke(const boost::json::value_to_tag<std::shared_ptr<ServerConfig>>&, boost::json::value const& json) {
    const boost::json::object& obj = json.as_object();

    return std::make_shared<ServerConfig>(
            extract<std::string>(obj, "Name"),
            extract<std::string>(obj, "Host"),
            extract<unsigned short>(obj, "Port"),
            extract<std::unordered_map<unsigned int, std::string>>(obj, "ErrorPages", {}),
            extract<unsigned int>(obj, "MaxBodySize", DEFAULT_MAX_BODY_SIZE),
            extract<unsigned int>(obj, "MaxRequestSize", DEFAULT_MAX_REQUEST_SIZE),
            extract<unsigned int>(obj, "KeepAliveTimeout_s", DEFAULT_KEEP_ALIVE_TIMEOUT),
            extract<unsigned int>(obj, "MaxKeepAliveTimeout_s", DEFAULT_MAX_KEEP_ALIVE_TIMEOUT),
            sequence_container_extract<std::vector<std::shared_ptr<Location>>>(obj, "Locations"));
}

Config tag_invoke(const boost::json::value_to_tag<Config>&, const boost::json::value& json) {
    const boost::json::object& obj = json.as_object();

    return {
            extract<unsigned int>(obj, "MaxSessionsNumber", DEFAULT_MAX_SESSIONS_NUMBER),
            extract<unsigned int>(obj, "ReadBufferSize", DEFAULT_MAX_SESSIONS_NUMBER),
            extract<unsigned int>(obj, "SessionsKillerDelay_s", DEFAULT_MAX_SESSIONS_NUMBER),
            extract<unsigned int>(obj, "HangSessionTimeout_s", DEFAULT_MAX_SESSIONS_NUMBER),
            PackToEndpoints(sequence_container_extract<std::vector<std::shared_ptr<ServerConfig>>>(obj, "Servers"))
    };
}

Config ConfigParser::Parse(const char* path) {
    try {
        auto const json = file_to_json(path);
        return boost::json::value_to<Config>(json);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Parse config error:\n"+ std::string(e.what()));
    }
}
