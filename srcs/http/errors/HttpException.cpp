#include "HttpException.h"

HttpException::HttpException(const std::string& message,
                             Http::Code error_code,
                             const std::string& error_title,
                             bool keep_alive,
                             const SharedPtr<ServerConfig>& server_instance_info)
        : _message(message),
          _keep_alive(keep_alive),
          _error_response(MakeShared(Response::MakeErrorResponse(error_code, error_title, server_instance_info))) {}

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
BadRequest::BadRequest(const std::string& message, const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad request", false, server_instance_info) {}

BadHttpVersion::BadHttpVersion(const std::string& message, const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad HTTP version", false, server_instance_info) {}

BadFirstLine::BadFirstLine(const std::string& message, const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad first line", false, server_instance_info) {}

BadHeader::BadHeader(const std::string& message, const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad header", false, server_instance_info) {}

BadContentLength::BadContentLength(const std::string& message, const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad content length", false, server_instance_info) {}

BadChunkSize::BadChunkSize(const std::string& message, const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad chunk size", false, server_instance_info) {}

BadChunkBody::BadChunkBody(const std::string& message, const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad chunk body", false, server_instance_info) {}


/// 404
NotFound::NotFound(const std::string& message, const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::NotFound, "Not found", true, server_instance_info) {}


/// 405
MethodNotAllowed::MethodNotAllowed(const std::string& message, const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::MethodNotAllowed, "Method not allowed", true, server_instance_info) {}


/// 411
LengthRequired::LengthRequired(const std::string& message, const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::LengthRequired, "Length required", false, server_instance_info) {}


/// 413
PayloadTooLarge::PayloadTooLarge(const std::string& message,
                                 const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::PayloadTooLarge, "Payload too large", false, server_instance_info) {}

/// 500
InternalServerError::InternalServerError(const std::string& message,
                                         const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::InternalServerError, "Internal server error", false, server_instance_info) {}

/// 501
NotImplemented::NotImplemented(const std::string& message, const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::NotImplemented, "Not implemented", true, server_instance_info) {}

UnsupportedTransferEncoding::UnsupportedTransferEncoding(const std::string& message,
                                                         const SharedPtr<ServerConfig>& server_instance_info)
        : HttpException(message, Http::NotImplemented, "Unsupported transfer encoding", false, server_instance_info) {}

