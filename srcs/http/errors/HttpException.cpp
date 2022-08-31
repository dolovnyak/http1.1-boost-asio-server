#include "HttpException.h"

HttpException::HttpException(const std::string& message,
                             Http::Code error_code,
                             const std::string& error_title,
                             const SharedPtr<ServerInfo>& server_instance_info)
        : _message(message),
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

/// 400
BadRequest::BadRequest(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad request", server_instance_info) {}

BadHttpVersion::BadHttpVersion(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad HTTP version", server_instance_info) {}

BadFirstLine::BadFirstLine(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad first line", server_instance_info) {}

BadHeader::BadHeader(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad header", server_instance_info) {}

BadContentLength::BadContentLength(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad content length", server_instance_info) {}

BadChunkSize::BadChunkSize(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad chunk size", server_instance_info) {}

BadChunkBody::BadChunkBody(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::BadRequest, "Bad chunk body", server_instance_info) {}


/// 404
NotFound::NotFound(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::NotFound, "Not found", server_instance_info) {}


/// 405
MethodNotAllowed::MethodNotAllowed(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::MethodNotAllowed, "Method not allowed", server_instance_info) {}


/// 411
LengthRequired::LengthRequired(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::LengthRequired, "Length required", server_instance_info) {}


/// 413
PayloadTooLarge::PayloadTooLarge(const std::string& message,
                                 const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::PayloadTooLarge, "Payload too large", server_instance_info) {}

/// 500
InternalServerError::InternalServerError(const std::string& message,
                                         const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::InternalServerError, "Internal server error", server_instance_info) {}

/// 501
NotImplemented::NotImplemented(const std::string& message, const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::NotImplemented, "Not implemented", server_instance_info) {}

UnsupportedTransferEncoding::UnsupportedTransferEncoding(const std::string& message,
                                                         const SharedPtr<ServerInfo>& server_instance_info)
        : HttpException(message, Http::NotImplemented, "Unsupported transfer encoding", server_instance_info) {}

