#pragma once

#include <exception>
#include "Optional.h"
#include "Response.h"
#include "SharedPtr.h"
#include "HttpErrorPages.h"

class HttpException : public std::exception {
public:
    HttpException(const std::string& message, Http::Code error_code, const std::string& error_title);

    HttpException(const HttpException& other) _NOEXCEPT;

    SharedPtr<Response> GetErrorResponse() const _NOEXCEPT;

    ~HttpException() _NOEXCEPT;

    const char* what() const _NOEXCEPT;

private:
    std::string _message;
    SharedPtr<Response> _error_response;
};

/// Bad request exceptions
class BadRequest : public HttpException {
public:
    BadRequest(const std::string& message);
};

class BadHttpVersion : public HttpException {
public:
    BadHttpVersion(const std::string& message);
};

class BadFirstLine : public HttpException {
public:
    BadFirstLine(const std::string& message);
};

class BadHeader : public HttpException {
public:
    BadHeader(const std::string& message);
};

class BadContentLength : public HttpException {
public:
    BadContentLength(const std::string& message);
};

class BadChunkSize : public HttpException {
public:
    BadChunkSize(const std::string& message);
};

class BadChunkBody : public HttpException {
public:
    BadChunkBody(const std::string& message);
};

/// Not implemented exceptions
class NotImplemented : public HttpException {
public:
    NotImplemented(const std::string& message);
};

class UnsupportedTransferEncoding : public HttpException {
public:
    UnsupportedTransferEncoding(const std::string& message);
};

/// Payload to large exception
class PayloadTooLarge : public HttpException {
public:
    PayloadTooLarge(const std::string& message);
};