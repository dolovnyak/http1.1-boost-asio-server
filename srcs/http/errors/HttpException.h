#pragma once

#include "Response.h"
#include "HttpErrorPages.h"
#include "Config.h"

#include <exception>

class HttpException : public std::exception {
public:
    HttpException(const std::string& message,
                  Http::Code error_code,
                  const std::string& error_title,
                  bool keep_alive,
                  const std::shared_ptr<ServerConfig>& server_config)
            : _message(message),
              _keep_alive(keep_alive),
              _error_response(std::make_shared(Response::MakeErrorResponse(error_code, error_title, server_config))) {}

    HttpException(const HttpException& other) noexcept {
        _message = other._message;
        _error_response = other._error_response;
    }

    [[nodiscard]] std::shared_ptr<Response> GetErrorResponse() const noexcept {
        return _error_response;
    }

    bool ShouldKeepAlive() const noexcept {
        return _keep_alive;
    }

    ~HttpException() noexcept {}

    [[nodiscard]] const char* what() const noexcept {
        return _message.c_str();
    }

private:
    std::string _message;
    bool _keep_alive;
    std::shared_ptr<Response> _error_response;
};

/// 400
class BadRequest : public HttpException {
public:
    BadRequest(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::BadRequest, "Bad request", false, server_config) {}
};

class BadHttpVersion : public HttpException {
public:
    BadHttpVersion(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::BadRequest, "Bad HTTP version", false, server_config) {}
};

class BadFirstLine : public HttpException {
public:
    BadFirstLine(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::BadRequest, "Bad first line", false, server_config) {}
};

class BadHeader : public HttpException {
public:
    BadHeader(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::BadRequest, "Bad header", false, server_config) {}
};

class BadContentLength : public HttpException {
public:
    BadContentLength(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::BadRequest, "Bad content length", false, server_config) {}
};

class BadChunkSize : public HttpException {
public:
    BadChunkSize(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::BadRequest, "Bad chunk size", false, server_config) {}
};

class BadChunkBody : public HttpException {
public:
    BadChunkBody(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::BadRequest, "Bad chunk body", false, server_config) {}
};


/// 404
class NotFound : public HttpException {
public:
    NotFound(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::NotFound, "Not found", true, server_config) {}
};


/// 405
class MethodNotAllowed : public HttpException {
public:
    MethodNotAllowed(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::MethodNotAllowed, "Method not allowed", true, server_config) {}
};


/// 411
class LengthRequired : public HttpException {
public:
    LengthRequired(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::LengthRequired, "Length required", false, server_config) {}
};


/// 413
class PayloadTooLarge : public HttpException {
public:
    PayloadTooLarge(const std::string& message,
                    const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::PayloadTooLarge, "Payload too large", false, server_config) {}
};


/// 500
class InternalServerError : public HttpException {
public:
    InternalServerError(const std::string& message,
                        const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::InternalServerError, "Internal server error", false, server_config) {}
};


/// 501
class NotImplemented : public HttpException {
public:
    NotImplemented(const std::string& message, const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::NotImplemented, "Not implemented", true, server_config) {}
};

class UnsupportedTransferEncoding : public HttpException {
public:
    UnsupportedTransferEncoding(const std::string& message,
                                const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::NotImplemented, "Unsupported transfer encoding", false, server_config) {}
};

/// 505
class HttpVersionNotSupported : public HttpException {
public:
    HttpVersionNotSupported(const std::string& message,
                            const std::shared_ptr<ServerConfig>& server_config)
            : HttpException(message, Http::Code::HttpVersionNotSupported, "HTTP version not supported", false,
                            server_config) {}
};
