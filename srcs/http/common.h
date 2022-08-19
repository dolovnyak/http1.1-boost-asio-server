#pragma once

#include <unordered_set>
#include <string>

#define CR                  (char) '\r'
#define LF                  (char) '\n'
#define CRLF                "\r\n"
#define SPACE_DELIMITERS    " \t\v"

#define CONTENT_LENGTH "content-length"
#define TRANSFER_ENCODING "transfer-encoding"

#define CHUNKED "chunked"

#define BAD_CONTENT_LENGTH_TITLE "400 Bad Content-Length"

//static std::unordered_set<std::string> allowed_methods = {
//    "GET",
//    "POST",
//    "DELETE",
//    "PUT",
//    "HEAD",
//    "OPTIONS",
//    "TRACE",
//    "CONNECT"
//};
