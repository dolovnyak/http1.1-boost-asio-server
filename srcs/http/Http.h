#pragma once

#include <string>

#define CR                  (char) '\r'
#define LF                  (char) '\n'
#define CRLF                "\r\n"
#define CRLF_LEN            2
#define DELIMITERS          " \f\n\r\t\v" /// from std::isspace

#define WEBSERVER_NAME "DolovServer/1.0"

#define PATH_TO_SAVE_CGI_KEY "PATH_TO_SAVE"

#define CONTENT_LENGTH "content-length"
#define CONTENT_TYPE "content-type"

#define DEFAULT_CONTENT_TYPE "text/html, charset=utf-8"

#define TRANSFER_ENCODING "transfer-encoding"
#define CHUNKED "chunked"

#define HOST "host"

#define CONNECTION "connection"
#define KEEP_ALIVE "keep-alive"

#define CLOSE "close"
#define TIMEOUT "timeout"

#define COOKIE "cookie"

namespace Http {

    class Header {
    public:
        Header(const std::string& key, const std::string& value)
                : key(key), value(value) {}

        std::string key;
        std::string value;
    };

    enum class Method {
        Get = 0,
        Head,
        Post,
        Delete,
        Put,
        Connect,
        Options,
        Trace,
        Patch
    };

    enum Version {
        Http1_0 = 0,
        Http1_1,
    };

    enum class Code {
        Continue = 100,
        SwitchingProtocols = 101,
        Ok = 200,
        Created = 201,
        Accepted = 202,
        NonAuthoritativeInformation = 203,
        NoContent = 204,
        ResetContent = 205,
        PartialContent = 206,
        MovedPermanently = 301,
        Found = 302,
        SeeOther = 303,
        TemporaryRedirect = 307,
        PermanentRedirect = 308,
        BadRequest = 400,
        AuthorizationRequired = 401,
        PaymentRequired = 402,
        Forbidden = 403,
        NotFound = 404,
        MethodNotAllowed = 405,
        NotAcceptable = 406,
        RequestTimeout = 408,
        Conflict = 409,
        Gone = 410,
        LengthRequired = 411,
        PreconditionFailed = 412,
        PayloadTooLarge = 413,
        UriTooLarge = 414,
        UnsupportedMedia = 415,
        RequestedRangeNotSatisfiable = 416,
        MisdirectedRequest = 421,
        TooManyRequests = 429,
        HeaderOrCookieTooLarge = 494,
        InternalServerError = 500,
        NotImplemented = 501,
        BadGateway = 502,
        TemporarilyUnavailable = 503,
        GatewayTimeout = 504,
        HttpVersionNotSupported = 505,
        InsufficientStorage = 507
    };

    enum class CodeType {
        Informational = 0,
        Success,
        Redirection,
        ClientError,
        ServerError
    };

    Method ToHttpMethod(const std::string& method);

    Code ToHttpCode(unsigned int code);

    CodeType GetCodeType(Code code);

    const std::string& ToString(Http::Version version);

    const std::string& ToString(Http::Method method);

    const std::string& ToString(Http::Code code);
}
