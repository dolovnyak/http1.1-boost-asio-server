#pragma once

#include <unordered_set>
#include <string>

#define CR     (char) '\r'
#define LF     (char) '\n'
#define CRLF   "\r\n"

#define CONTENT_LENGTH "content-length"

static std::unordered_set<std::string> allowed_methods = {
    "GET",
    "POST",
    "DELETE",
    "PUT",
    "HEAD",
    "OPTIONS",
    "TRACE",
    "CONNECT"
};

enum RequestErrorStatus {
    None = 0,
    BadRequest = 400,
    NotFound = 404,
    MethodNotAllowed = 405
};
