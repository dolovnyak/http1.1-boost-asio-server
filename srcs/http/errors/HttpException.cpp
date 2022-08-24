#include "HttpException.h"

HttpException::HttpException(const std::string& message, Http::Code error_code, const std::string& error_title)
        : _message(message),
          _error_response(MakeShared(Response::MakeErrorResponse(error_code, error_title))) {}

HttpException::~HttpException() _NOEXCEPT {}

const char* HttpException::what() const _NOEXCEPT {
    return _message.c_str();
}

HttpException::HttpException(const HttpException& other) _NOEXCEPT {
    _message = other._message;
    _error_response = other._error_response;
}

SharedPtr<Response> HttpException::GetErrorResponse() const _NOEXCEPT {
    return _error_response;
}

/// Bad request exceptions
BadRequest::BadRequest(const std::string& message)
        : HttpException(message, Http::BadRequest, "Bad request") {}

BadHttpVersion::BadHttpVersion(const std::string& message)
        : HttpException(message, Http::BadRequest, "Bad HTTP version") {}

BadFirstLine::BadFirstLine(const std::string& message)
        : HttpException(message, Http::BadRequest, "Bad first line") {}

BadHeader::BadHeader(const std::string& message)
        : HttpException(message, Http::BadRequest, "Bad header") {}

BadContentLength::BadContentLength(const std::string& message)
        : HttpException(message, Http::BadRequest, "Bad content length") {}

BadChunkSize::BadChunkSize(const std::string& message)
        : HttpException(message, Http::BadRequest, "Bad chunk size") {}

BadChunkBody::BadChunkBody(const std::string& message)
        : HttpException(message, Http::BadRequest, "Bad chunk body") {}


/// Not implemented exceptions
NotImplemented::NotImplemented(const std::string& message)
        : HttpException(message, Http::NotImplemented, "Not implemented") {}

UnsupportedTransferEncoding::UnsupportedTransferEncoding(const std::string& message)
        : HttpException(message, Http::NotImplemented, "Unsupported transfer encoding") {}


/// Payload to large exception
PayloadTooLarge::PayloadTooLarge(const std::string& message)
        : HttpException(message, Http::PayloadTooLarge, "Payload too large") {}