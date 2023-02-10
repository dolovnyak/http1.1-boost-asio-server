#include "ConfigParser.h"
#include "Config.h"
#include "Logging.h"

#include <boost/json/src.hpp>
#include <fstream>

struct RawErrorPage {
    unsigned int code;
    std::string path;
};

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
    T extract(boost::json::string_view key, const boost::json::object& obj) {
        auto it = obj.find(key);
        if (it == obj.end()) {
            throw std::runtime_error("required field \"" + std::string(key) + "\" is missing");
        }
        return boost::json::value_to<T>(it->value());
    }

    template<class T>
    T extract_soft(boost::json::string_view key, const boost::json::object& obj, const T& default_value) {
        auto it = obj.find(key);
        if (it == obj.end()) {
            return default_value;
        }
        return boost::json::value_to<T>(it->value());
    }

    template<class Container>
    Container sequence_container_extract(boost::json::string_view key, const boost::json::object& obj) {
        auto it = obj.find(key);
        if (it == obj.end()) {
            throw std::runtime_error("required field \"" + std::string(key) + "\" is missing");
        }
        const boost::json::array& arr = it->value().as_array();
        Container res;
        for (auto& arr_elem: arr) {
            res.emplace_back(boost::json::value_to<typename Container::value_type>(arr_elem));
        }
        return res;
    }

    template<class Container>
    Container sequence_container_extract_soft(boost::json::string_view key, const boost::json::object& obj,
                                              const Container& default_value) {
        auto it = obj.find(key);
        if (it == obj.end()) {
            return default_value;
        }
        const boost::json::array& arr = it->value().as_array();
        Container res;
        for (auto& arr_elem: arr) {
            res.emplace_back(boost::json::value_to<typename Container::value_type>(arr_elem));
        }
        return res;
    }

    template<class F, class ...Args>
    auto exceptions_wrapper(F f, boost::json::string_view key, Args... args) {
        try {
            return f(key, args...);
        }
        catch (const boost::system::system_error& e) {
            throw std::runtime_error("Error in field \"" + std::string(key) + "\"\n");
        }
        catch (const std::exception& e) {
            throw std::runtime_error("Error in field \"" + std::string(key) + "\"\n" + std::string(e.what()));
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

    std::unordered_map<Http::Code, std::string>
    ToErrorPages(std::vector<RawErrorPage>&& raw_error_pages) {
        std::unordered_map<Http::Code, std::string> error_pages;
        for (auto& raw_error_page: raw_error_pages) {
            error_pages.emplace(Http::ToHttpCode(raw_error_page.code), std::move(raw_error_page.path));
        }
        return error_pages;
    }

    std::vector<std::shared_ptr<Location>> ToSortedLocations(std::vector<std::shared_ptr<Location>>&& locations) {
        auto compare = [](const std::shared_ptr<Location>& left, const std::shared_ptr<Location>& right) {
            if (left->GetType() == LocationType::Prefix) {
                return false;
            }
            return true;
        };
        std::sort(locations.begin(), locations.end(), compare);
        return locations;
    }
}

HttpReturn tag_invoke(const boost::json::value_to_tag<HttpReturn>&, boost::json::value const& json) {
    const boost::json::object& obj = json.as_object();
    unsigned int http_code = exceptions_wrapper(extract<unsigned int>, "Code", obj);
    std::optional<std::string> redirect = exceptions_wrapper(extract_soft<std::optional<std::string>>, "Redirect", obj, std::nullopt);
    if (redirect.has_value() && (http_code < 300 || http_code > 399)) {
        throw std::runtime_error("redirect field only for redirect codes");
    }
    return {Http::ToHttpCode(http_code), redirect};
}

std::shared_ptr<Location>
tag_invoke(const boost::json::value_to_tag<std::shared_ptr<Location>>&, boost::json::value const& json) {
    const boost::json::object& obj = json.as_object();

    return std::make_shared<Location>(
            exceptions_wrapper(extract<std::string>, "Location", obj),
            exceptions_wrapper(extract_soft<std::optional<std::string>>, "Root", obj, std::nullopt),
            exceptions_wrapper(extract_soft<std::optional<std::string>>, "Cgi-path", obj, std::nullopt),
            exceptions_wrapper(extract_soft<std::optional<std::string>>, "Index", obj, std::nullopt),
            exceptions_wrapper(extract_soft<std::optional<HttpReturn>>, "Return", obj, std::nullopt),
            exceptions_wrapper(extract_soft<bool>, "Autoindex", obj, false),
            ToHandledMethods(exceptions_wrapper(sequence_container_extract_soft<std::vector<std::string>>,
                                                "AvailableMethods", obj, std::vector<std::string>())));
}

RawErrorPage
tag_invoke(const boost::json::value_to_tag<RawErrorPage>&, const boost::json::value& json) {
    const boost::json::object& obj = json.as_object();

    return {exceptions_wrapper(extract<unsigned int>, "Code", obj),
            exceptions_wrapper(extract<std::string>, "Path", obj)};
}

std::shared_ptr<ServerConfig>
tag_invoke(const boost::json::value_to_tag<std::shared_ptr<ServerConfig>>&, const boost::json::value& json) {
    const boost::json::object& obj = json.as_object();

    return std::make_shared<ServerConfig>(
            exceptions_wrapper(extract<std::string>, "Name", obj),
            exceptions_wrapper(extract<std::string>, "Host", obj),
            exceptions_wrapper(extract<unsigned short>, "Port", obj),
            ToErrorPages(exceptions_wrapper(
                    sequence_container_extract_soft<std::vector<RawErrorPage>>, "ErrorPages",
                    obj, std::vector<RawErrorPage>())),
            exceptions_wrapper(extract_soft<unsigned int>, "MaxBodySize", obj, DEFAULT_MAX_BODY_SIZE),
            exceptions_wrapper(extract_soft<unsigned int>, "MaxRequestSize", obj, DEFAULT_MAX_REQUEST_SIZE),
            exceptions_wrapper(extract_soft<unsigned int>, "KeepAliveTimeout_s", obj, DEFAULT_KEEP_ALIVE_TIMEOUT),
            exceptions_wrapper(extract_soft<unsigned int>, "MaxKeepAliveTimeout_s", obj,
                               DEFAULT_MAX_KEEP_ALIVE_TIMEOUT),
            ToSortedLocations(exceptions_wrapper(
                    sequence_container_extract<std::vector<std::shared_ptr<Location>>>, "Locations", obj)));
}

Config tag_invoke(const boost::json::value_to_tag<Config>&, const boost::json::value& json) {
    const boost::json::object& obj = json.as_object();

    return {
            exceptions_wrapper(extract_soft<unsigned int>, "MaxSessionsNumber", obj, DEFAULT_MAX_SESSIONS_NUMBER),
            PackToEndpoints(exceptions_wrapper(
                    sequence_container_extract<std::vector<std::shared_ptr<ServerConfig>>>, "Servers", obj))
    };
}

Config ConfigParser::Parse(const char* path) {
    try {
        auto const json = file_to_json(path);
        return boost::json::value_to<Config>(json);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Parse config error\n" + std::string(e.what()));
    }
}
