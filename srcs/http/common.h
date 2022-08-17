#pragma once

#define CR     (char) '\r'
#define LF     (char) '\n'
#define CRLF   "\r\n"

enum RequestErrorStatus {
    None = 0,
    BadRequest = 400,
    NotFound = 404,
    MethodNotAllowed = 405
};

