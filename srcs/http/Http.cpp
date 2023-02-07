#include "Http.h"
#include "utilities.h"

#include <unordered_map>

namespace Http {
    Method ToHttpMethod(const std::string& method) {

        static std::unordered_map<std::string, Method> methods_map = {
                {"GET",     Method::Get},
                {"POST",    Method::Post},
                {"HEAD",    Method::Head},
                {"PUT",     Method::Put},
                {"DELETE",  Method::Delete},
                {"CONNECT", Method::Connect},
                {"OPTIONS", Method::Options},
                {"TRACE",   Method::Trace},
                {"PATCH",   Method::Patch}};

        auto it = methods_map.find(ToUpper(method));
        if (it == methods_map.end()) {
            throw std::runtime_error("Method \"" + method + "\" is forbidden");
        }
        return it->second;
    }

    Code ToHttpCode(unsigned int code) {
        static std::unordered_map<unsigned int, Code> codes_map = {
                {200, Code::Ok},
                {400, Code::BadRequest},
                {404, Code::NotFound},
                {405, Code::MethodNotAllowed},
                {411, Code::LengthRequired},
                {413, Code::PayloadTooLarge},
                {500, Code::InternalServerError},
                {501, Code::NotImplemented},
                {505, Code::HttpVersionNotSupported}};
        auto it = codes_map.find(code);
        if (it == codes_map.end()) {
            throw std::runtime_error("Code \"" + std::to_string(code) + "\" is forbidden");
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