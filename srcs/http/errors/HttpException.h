#pragma once

#include "Response.h"
#include "SharedPtr.h"
#include "HttpErrorPages.h"
#include "ServerInfo.h"

#include <exception>

class HttpException : public std::exception {
public:
    HttpException(const std::string& message,
                  Http::Code error_code,
                  const std::string& error_title,
                  const SharedPtr<ServerInfo>& server_instance_info);

    HttpException(const HttpException& other) _NOEXCEPT;

    SharedPtr<Response> GetErrorResponse() const _NOEXCEPT;

    ~HttpException() _NOEXCEPT;

    const char* what() const _NOEXCEPT;

private:
    std::string _message;
    SharedPtr<Response> _error_response;
};

/// 400
class BadRequest : public HttpException {
public:
    BadRequest(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info);
};

class BadHttpVersion : public HttpException {
public:
    BadHttpVersion(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info);
};

class BadFirstLine : public HttpException {
public:
    BadFirstLine(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info);
};

class BadHeader : public HttpException {
public:
    BadHeader(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info);
};

class BadContentLength : public HttpException {
public:
    BadContentLength(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info);
};

class BadChunkSize : public HttpException {
public:
    BadChunkSize(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info);
};

class BadChunkBody : public HttpException {
public:
    BadChunkBody(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info);
};


/// 405
class MethodNotAllowed : public HttpException {
public:
    MethodNotAllowed(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info);
};


/// 413
class PayloadTooLarge : public HttpException {
public:
    PayloadTooLarge(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info);
};


/// 500
class InternalServerError : public HttpException {
public:
    InternalServerError(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info);
};


/// 501
class NotImplemented : public HttpException {
public:
    NotImplemented(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info);
};

class UnsupportedTransferEncoding : public HttpException {
public:
    UnsupportedTransferEncoding(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info);
};
