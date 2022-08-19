#include "HttpException.h"

HttpException::HttpException(const std::string& message, HttpError::Code error)
: _message(message), _title(), _error(error) {}

HttpException::HttpException(const std::string& message, const std::string& title, HttpError::Code error)
: _message(message), _title(title), _error(error) {}

HttpException::~HttpException() _NOEXCEPT {}

HttpError::Code HttpException::GetError() const _NOEXCEPT {
    return _error;
}

const char* HttpException::what() const _NOEXCEPT {
    return _message.c_str();
}

HttpException::HttpException(const HttpException& other) _NOEXCEPT {
    _message = other._message;
    _error = other._error;
}


