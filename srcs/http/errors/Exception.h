#pragma once

#include "Response.h"
#include "ErrorPages.h"
#include "Config.h"

#include <exception>
#include <utility>

namespace Http {

class Exception : public std::exception {
public:
    Exception(std::string  message,
              Http::Code error_code,
              const std::string& error_title,
              bool keep_alive,
              const std::shared_ptr<ServerConfig>& server_config)
            : _message(std::move(message)),
              _keep_alive(keep_alive),
              _error_response(Response::MakeErrorResponse(error_code, error_title, server_config, {})) {}

    Exception(const Exception& other) noexcept {
        _message = other._message;
        _error_response = other._error_response;
    }

    [[nodiscard]] std::shared_ptr<Response> GetErrorResponse() const noexcept {
        return _error_response;
    }

    bool ShouldKeepAlive() const noexcept {
        return _keep_alive;
    }

    ~Exception() noexcept {}

    [[nodiscard]] const char* what() const noexcept {
        return _message.c_str();
    }

    static const Exception& GetByCode(Code code, const std::shared_ptr<ServerConfig>& server_config);


private:
    std::string _message;
    bool _keep_alive;
    std::shared_ptr<Response> _error_response;
};

/// 400
class BadRequest : public Exception {
public:
    BadRequest(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::BadRequest, ToString(Http::Code::BadRequest), false, server_config) {}
};

class BadHttpVersion : public Exception {
public:
    BadHttpVersion(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::BadRequest, "Bad HTTP version", false, server_config) {}
};

class BadFirstLine : public Exception {
public:
    BadFirstLine(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::BadRequest, "Bad first line", false, server_config) {}
};

class BadHeader : public Exception {
public:
    BadHeader(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::BadRequest, "Bad header", false, server_config) {}
};

class BadContentLength : public Exception {
public:
    BadContentLength(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::BadRequest, "Bad content length", false, server_config) {}
};

class BadChunkSize : public Exception {
public:
    BadChunkSize(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::BadRequest, "Bad chunk size", false, server_config) {}
};

class BadChunkBody : public Exception {
public:
    BadChunkBody(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::BadRequest, "Bad chunk body", false, server_config) {}
};

/// 401
class AuthorizationRequired : public Exception {

public:
    AuthorizationRequired(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::AuthorizationRequired, ToString(Http::Code::AuthorizationRequired), true, server_config) {}
};

/// 402
class PaymentRequired : public Exception {
public:
    PaymentRequired(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::PaymentRequired, ToString(Http::Code::PaymentRequired), true, server_config) {}
};

/// 403
class Forbidden : public Exception {
public:
    Forbidden(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::Forbidden, ToString(Http::Code::Forbidden), true, server_config) {}
};

/// 404
class NotFound : public Exception {
public:
    NotFound(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::NotFound, ToString(Http::Code::NotFound), true, server_config) {}
};


/// 405
class MethodNotAllowed : public Exception {
public:
    MethodNotAllowed(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::MethodNotAllowed, ToString(Http::Code::MethodNotAllowed), true, server_config) {}
};

/// 406
class NotAcceptable : public Exception {
public:
    NotAcceptable(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::NotAcceptable, ToString(Http::Code::NotAcceptable), false, server_config) {}
};

/// 408
class RequestTimeout : public Exception {
public:
    RequestTimeout(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::RequestTimeout, ToString(Http::Code::RequestTimeout), false, server_config) {}
};

/// 409
class Conflict : public Exception {
public:
    Conflict(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::Conflict, ToString(Http::Code::Conflict), false, server_config) {}
};

/// 410
class Gone : public Exception {
public:
    Gone(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::Gone, ToString(Http::Code::Gone), false, server_config) {}
};

/// 411
class LengthRequired : public Exception {
public:
    LengthRequired(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::LengthRequired, ToString(Http::Code::LengthRequired), false, server_config) {}
};

/// 412
class PreconditionFailed : public Exception {
public:
    PreconditionFailed(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::PreconditionFailed, ToString(Http::Code::PreconditionFailed), false, server_config) {}
};

/// 413
class PayloadTooLarge : public Exception {
public:
    PayloadTooLarge(const std::string& message,
                    const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::PayloadTooLarge, ToString(Http::Code::PayloadTooLarge), false, server_config) {}
};

/// 414
class UriTooLarge : public Exception {
public:
    UriTooLarge(const std::string& message,
                const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::UriTooLarge, ToString(Http::Code::UriTooLarge), false, server_config) {}
};

/// 415
class UnsupportedMedia : public Exception {
public:
    UnsupportedMedia(const std::string& message,
                     const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::UnsupportedMedia, ToString(Http::Code::UnsupportedMedia), false, server_config) {}
};

/// 416
class RequestedRangeNotSatisfiable : public Exception {
public:
    RequestedRangeNotSatisfiable(const std::string& message,
                                 const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::RequestedRangeNotSatisfiable, ToString(Http::Code::RequestedRangeNotSatisfiable), false,
                        server_config) {}
};

/// 421
class MisdirectedRequest : public Exception {
public:
    MisdirectedRequest(const std::string& message,
                       const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::MisdirectedRequest, ToString(Http::Code::MisdirectedRequest), false, server_config) {}
};

/// 429
class TooManyRequests : public Exception {
public:
    TooManyRequests(const std::string& message,
                    const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::TooManyRequests, ToString(Http::Code::TooManyRequests), false, server_config) {}
};

/// 494
class HeaderOrCookieTooLarge : public Exception {
public:
    HeaderOrCookieTooLarge(const std::string& message,
                           const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::HeaderOrCookieTooLarge, ToString(Http::Code::HeaderOrCookieTooLarge), false,
                        server_config) {}
};

/// 500
class InternalServerError : public Exception {
public:
    InternalServerError(const std::string& message,
                        const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::InternalServerError, ToString(Http::Code::InternalServerError), false,
                        server_config) {}
};

/// 501
class NotImplemented : public Exception {
public:
    NotImplemented(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::NotImplemented, ToString(Http::Code::NotImplemented), true, server_config) {}
};

class UnsupportedTransferEncoding : public Exception {
public:
    UnsupportedTransferEncoding(const std::string& message,
                                const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::NotImplemented, "Unsupported Transfer Encoding", false,
                        server_config) {}
};

/// 502
class BadGateway : public Exception {
public:
    BadGateway(const std::string& message,
               const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::BadGateway, ToString(Http::Code::BadGateway), false,
                        server_config) {}
};

/// 503
class TemporarilyUnavailable : public Exception {
public:
    TemporarilyUnavailable(const std::string& message,
                           const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::TemporarilyUnavailable, ToString(Http::Code::TemporarilyUnavailable), false, server_config) {}
};

/// 504
class GatewayTimeout : public Exception {
public:
    GatewayTimeout(const std::string& message,
                   const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::GatewayTimeout, ToString(Http::Code::GatewayTimeout), false, server_config) {}
};

/// 505
class VersionNotSupported : public Exception {
public:
    VersionNotSupported(const std::string& message,
                        const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::HttpVersionNotSupported, ToString(Http::Code::HttpVersionNotSupported), false,
                        server_config) {}
};

/// 507
class InsufficientStorage : public Exception {
public:
    InsufficientStorage(const std::string& message,
                        const std::shared_ptr<ServerConfig>& server_config)
            : Exception(message, Http::Code::InsufficientStorage, ToString(Http::Code::InsufficientStorage), false,
                        server_config) {}
};

inline const Exception& Exception::GetByCode(Code code, const std::shared_ptr<ServerConfig>& server_config) {
    static std::unordered_map<Code, Exception> exceptions = {
            {Code::BadRequest,                   BadRequest("", server_config)},
            {Code::AuthorizationRequired,        AuthorizationRequired("", server_config)},
            {Code::PaymentRequired,              PaymentRequired("", server_config)},
            {Code::Forbidden,                    Forbidden("", server_config)},
            {Code::NotFound,                     NotFound("", server_config)},
            {Code::MethodNotAllowed,             MethodNotAllowed("", server_config)},
            {Code::NotAcceptable,                NotAcceptable("", server_config)},
            {Code::RequestTimeout,               RequestTimeout("", server_config)},
            {Code::Conflict,                     Conflict("", server_config)},
            {Code::Gone,                         Gone("", server_config)},
            {Code::LengthRequired,               LengthRequired("", server_config)},
            {Code::PreconditionFailed,           PreconditionFailed("", server_config)},
            {Code::PayloadTooLarge,              PayloadTooLarge("", server_config)},
            {Code::UriTooLarge,                  UriTooLarge("", server_config)},
            {Code::RequestedRangeNotSatisfiable, RequestedRangeNotSatisfiable("", server_config)},
            {Code::MisdirectedRequest,           MisdirectedRequest("", server_config)},
            {Code::TooManyRequests,              TooManyRequests("", server_config)},
            {Code::HeaderOrCookieTooLarge,       HeaderOrCookieTooLarge("", server_config)},
            {Code::InternalServerError,          InternalServerError("", server_config)},
            {Code::NotImplemented,               NotImplemented("", server_config)},
            {Code::BadGateway,                   BadGateway("", server_config)},
            {Code::TemporarilyUnavailable,       TemporarilyUnavailable("", server_config)},
            {Code::GatewayTimeout,               GatewayTimeout("", server_config)},
            {Code::HttpVersionNotSupported,      VersionNotSupported("", server_config)},
            {Code::InsufficientStorage,          InsufficientStorage("", server_config)},
    };

    auto it = exceptions.find(code);
    if (it == exceptions.end()) {
        throw std::logic_error("there is no exception by code \"" + std::to_string(static_cast<int>(code)) + "\"");

    }
    return it->second;
}

}