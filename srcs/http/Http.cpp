#include "Http.h"
#include "utilities.h"

#include <unordered_map>

namespace Http {
    Method GetMethod(const std::string& method) {

        static std::unordered_map<std::string, Method> methods_map = {
                {"Get",     Method::Get},
                {"Post",    Method::Post},
                {"Head",    Method::Head},
                {"Put",     Method::Put},
                {"Delete",  Method::Delete},
                {"Connect", Method::Connect},
                {"Options", Method::Options},
                {"Trace",   Method::Trace},
                {"Patch",   Method::Patch}};

        auto it = methods_map.find(method);
        if (it == methods_map.end()) {
            return Method::Unknown;
        }
        return it->second;
    }

    const std::string& ToString(Http::Version http_version) {
        static std::unordered_map<Http::Version, std::string> version_map = {
                {Http1_0, "HTTP/1.0"},
                {Http1_1, "HTTP/1.1"}};
        return version_map[http_version];
    }
}