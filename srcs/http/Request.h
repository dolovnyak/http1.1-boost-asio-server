#pragma once

#include <string>
#include <optional>

enum class RequestStatus {
    Success = 0,
    BadRequest, // 400
    NotFound, // 404
    NotAllowed // 405
};

class Headers {
public:
    std::optional<int32_t> content_length;
    int32_t dick_length = 25;
};

class Request {
public:
    Headers headers;

    std::string body;

    RequestStatus status;

private:
    bool _is_body;
};
