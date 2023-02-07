#include "HttpErrorPages.h"

#include <unordered_map>

namespace {
    const std::string k301 =
            "<html>" CRLF
            "<head><title>301 Moved Permanently</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>301 Moved Permanently</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k302 =
            "<html>" CRLF
            "<head><title>302 Found</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>302 Found</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k303 =
            "<html>" CRLF
            "<head><title>303 See Other</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>303 See Other</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k307 =
            "<html>" CRLF
            "<head><title>307 Temporary Redirect</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>307 Temporary Redirect</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k308 =
            "<html>" CRLF
            "<head><title>308 Permanent Redirect</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>308 Permanent Redirect</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k400 =
            "<html>" CRLF
            "<head><title>400 Bad Request</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>400 Bad Request</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k401 =
            "<html>" CRLF
            "<head><title>401 Authorization Required</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>401 Authorization Required</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k402 =
            "<html>" CRLF
            "<head><title>402 Payment Required</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>402 Payment Required</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k403 =
            "<html>" CRLF
            "<head><title>403 Forbidden</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>403 Forbidden</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k404 =
            "<html>" CRLF
            "<head><title>404 Not Found</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>404 Not Found</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k405 =
            "<html>" CRLF
            "<head><title>405 Not Allowed</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>405 Not Allowed</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k406 =
            "<html>" CRLF
            "<head><title>406 Not Acceptable</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>406 Not Acceptable</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k408 =
            "<html>" CRLF
            "<head><title>408 Request Time-out</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>408 Request Time-out</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k409 =
            "<html>" CRLF
            "<head><title>409 Conflict</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>409 Conflict</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k410 =
            "<html>" CRLF
            "<head><title>410 Gone</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>410 Gone</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k411 =
            "<html>" CRLF
            "<head><title>411 Length Required</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>411 Length Required</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k412 =
            "<html>" CRLF
            "<head><title>412 Precondition Failed</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>412 Precondition Failed</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k413 =
            "<html>" CRLF
            "<head><title>413 Request Entity Too Large</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>413 Request Entity Too Large</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k414 =
            "<html>" CRLF
            "<head><title>414 Request-URI Too Large</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>414 Request-URI Too Large</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k415 =
            "<html>" CRLF
            "<head><title>415 Unsupported Media Type</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>415 Unsupported Media Type</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k416 =
            "<html>" CRLF
            "<head><title>416 Requested Range Not Satisfiable</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>416 Requested Range Not Satisfiable</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k421 =
            "<html>" CRLF
            "<head><title>421 Misdirected Request</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>421 Misdirected Request</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k429 =
            "<html>" CRLF
            "<head><title>429 Too Many Requests</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>429 Too Many Requests</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k494 =
            "<html>" CRLF
            "<head><title>400 Request Header Or Cookie Too Large</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>400 Bad Request</h1></center>" CRLF
            "<center>Request Header Or Cookie Too Large</center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k495 =
            "<html>" CRLF
            "<head><title>400 The SSL certificate error</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>400 Bad Request</h1></center>" CRLF
            "<center>The SSL certificate error</center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k496 =
            "<html>" CRLF
            "<head><title>400 No required SSL certificate was sent</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>400 Bad Request</h1></center>" CRLF
            "<center>No required SSL certificate was sent</center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k497 =
            "<html>" CRLF
            "<head><title>400 The plain HTTP request was sent to HTTPS port</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>400 Bad Request</h1></center>" CRLF
            "<center>The plain HTTP request was sent to HTTPS port</center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k500 =
            "<html>" CRLF
            "<head><title>500 Internal Server Error</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>500 Internal Server Error</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k501 =
            "<html>" CRLF
            "<head><title>501 Not Implemented</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>501 Not Implemented</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k502 =
            "<html>" CRLF
            "<head><title>502 Bad Gateway</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>502 Bad Gateway</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k503 =
            "<html>" CRLF
            "<head><title>503 Service Temporarily Unavailable</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>503 Service Temporarily Unavailable</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k504 =
            "<html>" CRLF
            "<head><title>504 Gateway Time-out</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>504 Gateway Time-out</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k505 =
            "<html>" CRLF
            "<head><title>505 HTTP Version Not Supported</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>505 HTTP Version Not Supported</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;


    const std::string k507 =
            "<html>" CRLF
            "<head><title>507 Insufficient Storage</title></head>" CRLF
            "<body>" CRLF
            "<center><h1>507 Insufficient Storage</h1></center>" CRLF
            "</body>" CRLF
            "</html>" CRLF;
}


const std::string& GetHttpErrorPageByCode(Http::Code code) {
    static std::unordered_map<unsigned int, std::string> error_pages = {
            {400, k400},
            {401, k401},
            {402, k402},
            {403, k403},
            {404, k404},
            {405, k405},
            {406, k406},
            {408, k408},
            {409, k409},
            {410, k410},
            {411, k411},
            {412, k412},
            {413, k413},
            {414, k414},
            {415, k415},
            {416, k416},
            {421, k421},
            {429, k429},
            {494, k494},
            {495, k495},
            {496, k496},
            {497, k497},
            {500, k500},
            {501, k501},
            {502, k502},
            {503, k503},
            {504, k504},
            {505, k505},
            {507, k507}
    };
    const std::unordered_map<unsigned int, std::string>::iterator& it = error_pages.find(static_cast<unsigned int>(code));
    if (it != error_pages.end()) {
        return it->second;
    }
    throw std::logic_error("Unknown http error code");
}

std::string GetHttpErrorPageByCode(Http::Code code, const std::shared_ptr<ServerConfig>& server_config) {
    auto it = server_config->error_pages.find(code);

    if (it != server_config->error_pages.end()) {
        std::string error_page;
        if (ReadFile(it->second, error_page)) {
            return error_page;
        }
    }

    return GetHttpErrorPageByCode(code);
}
