#include "ConfigParser.h"
#include "Config.h"

#include <boost/json/src.hpp>
#include <boost/property_tree/ptree.hpp>

#include <boost/property_tree/json_parser.hpp>

namespace {
    boost::json::value file_to_json(char const* filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Can't open json file");
        }

        boost::json::stream_parser p;
        boost::json::error_code ec;

        while (!file.eof()) {
            char buf[JSON_READ_BUFFER_SIZE];
            file.read(buf, sizeof(buf));
            size_t extracted_size = file.gcount();

            std::string a(buf, extracted_size);

            p.write(buf, extracted_size, ec);
        }

        if (ec) {
            throw std::runtime_error("Can't parse json file");
        }

        p.finish(ec);

        if (ec) {
            throw std::runtime_error("Can't parse json file");
        }

        return p.release();
    }

    template<class T>
    T extract(const boost::json::object& obj, boost::json::string_view key, const T& default_value) {
        auto it = obj.find(key);
        if (it == obj.end()) {
            return default_value;
        }
        return boost::json::value_to<T>(obj.at(key));
    }

    template<class T>
    T extract(const boost::json::object& obj, boost::json::string_view key) {
        return boost::json::value_to<T>(obj.at(key));
    }

    template<class Container>
    Container sequence_container_extract(const boost::json::object& obj, boost::json::string_view key) {
        Container res;

        const boost::json::array& arr = obj.at(key).as_array();

        for (auto& arr_elem: arr) {
            res.emplace_back(boost::json::value_to<typename Container::value_type>(arr_elem));
        }
        return res;
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
            sequence_container_extract<std::vector<std::string>>(obj, "AvailableMethods"),
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

    std::vector<int> a;

    return std::make_shared<ServerConfig>(
            extract<std::string>(obj, "Name"),
            extract<std::string>(obj, "Host"),
            extract<unsigned int>(obj, "Port"),
            extract<std::unordered_map<unsigned int, std::string>>(obj, "ErrorPages"),
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
            sequence_container_extract<std::vector<std::shared_ptr<ServerConfig>>>(obj, "Servers")
    };
}

Config ConfigParser::Parse(const char* path) {
    auto const json = file_to_json(path);
    return boost::json::value_to<Config>(json);
}
