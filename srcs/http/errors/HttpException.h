#pragma once

#include <exception>
#include "common.h"
#include "Optional.h"

namespace HttpError {
    enum Code {
        BadRequest = 400,
        NotFound = 404,
        MethodNotAllowed = 405,
        NotImplemented = 501,
    };
}

class HttpException : public std::exception {
public:
    HttpException(const std::string& message,
                  HttpError::Code error);

    HttpException(const std::string& message,
                  const std::string& title,
                  HttpError::Code error);

    HttpException(const HttpException& other) _NOEXCEPT;

    ~HttpException() _NOEXCEPT;

    const char* what() const _NOEXCEPT;

    HttpError::Code GetError() const _NOEXCEPT;

private:
    std::string _message;
    Optional<std::string> _title;
    HttpError::Code _error;
};
