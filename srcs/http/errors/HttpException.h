#pragma once

#include "Response.h"
#include "SharedPtr.h"
#include "HttpErrorPages.h"
#include "Config.h"
#include "Optional.h"

#include <exception>

class HttpException : public std::exception {
public:
    HttpException(const std::string& message,
                  Http::Code error_code,
                  const std::string& error_title,
                  bool keep_alive,
                  const Optional<SharedPtr<ServerConfig> >& server_config);

    HttpException(const HttpException& other) _NOEXCEPT;

    SharedPtr<Response> GetErrorResponse() const _NOEXCEPT;

    bool ShouldKeepAlive() const _NOEXCEPT;

    ~HttpException() _NOEXCEPT;

    const char* what() const _NOEXCEPT;

private:
    std::string _message;
    bool _keep_alive;
    SharedPtr<Response> _error_response;
};

/// 400
class BadRequest : public HttpException {
public:
    BadRequest(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};

class BadHttpVersion : public HttpException {
public:
    BadHttpVersion(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};

class BadFirstLine : public HttpException {
public:
    BadFirstLine(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};

class BadHeader : public HttpException {
public:
    BadHeader(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};

class BadContentLength : public HttpException {
public:
    BadContentLength(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};

class BadChunkSize : public HttpException {
public:
    BadChunkSize(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};

class BadChunkBody : public HttpException {
public:
    BadChunkBody(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};


/// 404
class NotFound : public HttpException {
public:
    NotFound(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};


/// 405
class MethodNotAllowed : public HttpException {
public:
    MethodNotAllowed(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};


/// 411
class LengthRequired : public HttpException {
public:
    LengthRequired(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};


/// 413
class PayloadTooLarge : public HttpException {
public:
    PayloadTooLarge(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};


/// 500
class InternalServerError : public HttpException {
public:
    InternalServerError(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};


/// 501
class NotImplemented : public HttpException {
public:
    NotImplemented(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};

class UnsupportedTransferEncoding : public HttpException {
public:
    UnsupportedTransferEncoding(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};

/// 505
class HttpVersionNotSupported : public HttpException {
public:
    HttpVersionNotSupported(const std::string& message,  const Optional<SharedPtr<ServerConfig> >& server_config);
};
