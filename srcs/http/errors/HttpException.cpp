#include "HttpException.h"

HttpException::HttpException(const std::string& message,
                             Http::Code error_code,
                             const std::string& error_title,
                             bool keep_alive,
                             const Optional<SharedPtr<ServerConfig> >& server_config)
        : _message(message),
          _keep_alive(keep_alive),
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

bool HttpException::ShouldKeepAlive() const _NOEXCEPT {
    return _keep_alive;
}

/// 400
BadRequest::BadRequest(const std::string& message, const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::BadRequest, "Bad request", false, server_config) {}

BadHttpVersion::BadHttpVersion(const std::string& message, const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::BadRequest, "Bad HTTP version", false, server_config) {}

BadFirstLine::BadFirstLine(const std::string& message, const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::BadRequest, "Bad first line", false, server_config) {}

BadHeader::BadHeader(const std::string& message, const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::BadRequest, "Bad header", false, server_config) {}

BadContentLength::BadContentLength(const std::string& message, const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::BadRequest, "Bad content length", false, server_config) {}

BadChunkSize::BadChunkSize(const std::string& message, const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::BadRequest, "Bad chunk size", false, server_config) {}

BadChunkBody::BadChunkBody(const std::string& message, const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::BadRequest, "Bad chunk body", false, server_config) {}


/// 404
NotFound::NotFound(const std::string& message, const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::NotFound, "Not found", true, server_config) {}


/// 405
MethodNotAllowed::MethodNotAllowed(const std::string& message, const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::MethodNotAllowed, "Method not allowed", true, server_config) {}


/// 411
LengthRequired::LengthRequired(const std::string& message, const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::LengthRequired, "Length required", false, server_config) {}


/// 413
PayloadTooLarge::PayloadTooLarge(const std::string& message,
                                 const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::PayloadTooLarge, "Payload too large", false, server_config) {}

/// 500
InternalServerError::InternalServerError(const std::string& message,
                                         const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::InternalServerError, "Internal server error", false, server_config) {}

/// 501
NotImplemented::NotImplemented(const std::string& message, const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::NotImplemented, "Not implemented", true, server_config) {}

UnsupportedTransferEncoding::UnsupportedTransferEncoding(const std::string& message,
                                                         const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::NotImplemented, "Unsupported transfer encoding", false, server_config) {}


/// 505
HttpVersionNotSupported::HttpVersionNotSupported(const std::string& message,
                                                 const SharedPtr<ServerConfig>& server_config)
        : HttpException(message, Http::HttpVersionNotSupported, "HTTP version not supported", false,
                        server_config) {}