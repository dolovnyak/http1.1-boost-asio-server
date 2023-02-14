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
            {100, Code::Continue},
            {101, Code::SwitchingProtocols},
            {200, Code::Ok},
            {201, Code::Created},
            {202, Code::Accepted},
            {203, Code::NonAuthoritativeInformation},
            {204, Code::NoContent},
            {205, Code::ResetContent},
            {206, Code::PartialContent},
            {301, Code::MovedPermanently},
            {302, Code::Found},
            {303, Code::SeeOther},
            {307, Code::TemporaryRedirect},
            {308, Code::PermanentRedirect},
            {400, Code::BadRequest},
            {401, Code::AuthorizationRequired},
            {402, Code::PaymentRequired},
            {403, Code::Forbidden},
            {404, Code::NotFound},
            {405, Code::MethodNotAllowed},
            {406, Code::NotAcceptable},
            {408, Code::RequestTimeout},
            {409, Code::Conflict},
            {410, Code::Gone},
            {411, Code::LengthRequired},
            {412, Code::PreconditionFailed},
            {413, Code::PayloadTooLarge},
            {414, Code::UriTooLarge},
            {415, Code::UnsupportedMedia},
            {416, Code::RequestedRangeNotSatisfiable},
            {421, Code::MisdirectedRequest},
            {429, Code::TooManyRequests},
            {494, Code::HeaderOrCookieTooLarge},
            {500, Code::InternalServerError},
            {501, Code::NotImplemented},
            {502, Code::BadGateway},
            {503, Code::TemporarilyUnavailable},
            {504, Code::GatewayTimeout},
            {505, Code::HttpVersionNotSupported},
            {507, Code::InsufficientStorage}};
    auto it = codes_map.find(code);
    if (it == codes_map.end()) {
        throw std::runtime_error("Code \"" + std::to_string(code) + "\" is forbidden");
    }
    return it->second;
}

CodeType GetCodeType(Code code) {
    if (static_cast<int>(code) < 200) {
        return CodeType::Informational;
    }
    if (static_cast<int>(code) < 300) {
        return CodeType::Success;
    }
    if (static_cast<int>(code) < 400) {
        return CodeType::Redirection;
    }
    if (static_cast<int>(code) < 500) {
        return CodeType::ClientError;
    }
    if (static_cast<int>(code) < 600) {
        return CodeType::ServerError;
    }
    throw std::logic_error("Code should be lower than 600");
}

const std::string& ToString(Http::Version version) {
    static std::unordered_map<Http::Version, std::string> versions_map = {
            {Http1_0, "HTTP/1.0"},
            {Http1_1, "HTTP/1.1"}};
    return versions_map[version];
}

const std::string& ToString(Http::Method method) {
    static std::unordered_map<Http::Method, std::string> methods_map = {
            {Method::Get,     "Get"},
            {Method::Head,    "Head"},
            {Method::Post,    "Post"},
            {Method::Delete,  "Delete"},
            {Method::Put,     "Put"},
            {Method::Connect, "Connect"},
            {Method::Options, "Options"},
            {Method::Trace,   "Trace"},
            {Method::Patch,   "Patch"}};
    return methods_map[method];
}

const std::string& ToString(Http::Code code) {
    static std::unordered_map<Http::Code, std::string> codes_map = {
            {Code::Continue,                     "Continue"},
            {Code::SwitchingProtocols,           "Switching Protocols"},
            {Code::Ok,                           "Ok"},
            {Code::Created,                      "Created"},
            {Code::Accepted,                     "Accepted"},
            {Code::NonAuthoritativeInformation,  "Non Authoritative Information"},
            {Code::NoContent,                    "No Content"},
            {Code::ResetContent,                 "Reset Content"},
            {Code::PartialContent,               "Partial Content"},
            {Code::MovedPermanently,             "Moved Permanently"},
            {Code::Found,                        "Found"},
            {Code::SeeOther,                     "See Other"},
            {Code::TemporaryRedirect,            "Temporary Redirect"},
            {Code::PermanentRedirect,            "Permanent Redirect"},
            {Code::BadRequest,                   "Bad Request"},
            {Code::AuthorizationRequired,        "Authorization Required"},
            {Code::PaymentRequired,              "Payment Required"},
            {Code::Forbidden,                    "Forbidden"},
            {Code::NotFound,                     "Not Found"},
            {Code::MethodNotAllowed,             "Method Not Allowed"},
            {Code::NotAcceptable,                "Not Acceptable"},
            {Code::RequestTimeout,               "Request Timeout"},
            {Code::Conflict,                     "Conflict"},
            {Code::Gone,                         "Gone"},
            {Code::LengthRequired,               "Length Required"},
            {Code::PreconditionFailed,           "Precondition Failed"},
            {Code::PayloadTooLarge,              "Payload Too Large"},
            {Code::UriTooLarge,                  "Uri Too Large"},
            {Code::RequestedRangeNotSatisfiable, "Requested Range Not Satisfiable"},
            {Code::MisdirectedRequest,           "Misdirected Request"},
            {Code::TooManyRequests,              "Too Many Requests"},
            {Code::HeaderOrCookieTooLarge,       "Header Or Cookie Too Large"},
            {Code::InternalServerError,          "Internal Server Error"},
            {Code::NotImplemented,               "Not Implemented"},
            {Code::BadGateway,                   "Bad Gateway"},
            {Code::TemporarilyUnavailable,       "Temporarily Unavailable"},
            {Code::GatewayTimeout,               "Gateway Timeout"},
            {Code::HttpVersionNotSupported,      "Http Version Not Supported"},
            {Code::InsufficientStorage,          "Insufficient Storage"}};
    return codes_map[code];
}

}