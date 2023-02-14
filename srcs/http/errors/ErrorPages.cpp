#include "ErrorPages.h"

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

namespace Http {

const std::string& GetHttpErrorPageByCode(Http::Code code) {
    static std::unordered_map<Http::Code, std::string> error_pages = {
            {Http::Code::MovedPermanently, k301},
            {Http::Code::Found, k302},
            {Http::Code::SeeOther, k303},
            {Http::Code::TemporaryRedirect, k307},
            {Http::Code::PermanentRedirect, k308},
            {Http::Code::BadRequest, k400},
            {Http::Code::AuthorizationRequired, k401},
            {Http::Code::PaymentRequired, k402},
            {Http::Code::Forbidden, k403},
            {Http::Code::NotFound, k404},
            {Http::Code::MethodNotAllowed, k405},
            {Http::Code::NotAcceptable, k406},
            {Http::Code::RequestTimeout, k408},
            {Http::Code::Conflict, k409},
            {Http::Code::Gone, k410},
            {Http::Code::LengthRequired, k411},
            {Http::Code::PreconditionFailed, k412},
            {Http::Code::PayloadTooLarge, k413},
            {Http::Code::UriTooLarge, k414},
            {Http::Code::UnsupportedMedia, k415},
            {Http::Code::RequestedRangeNotSatisfiable, k416},
            {Http::Code::MisdirectedRequest, k421},
            {Http::Code::TooManyRequests, k429},
            {Http::Code::HeaderOrCookieTooLarge, k494},
            {Http::Code::InternalServerError, k500},
            {Http::Code::NotImplemented, k501},
            {Http::Code::BadGateway, k502},
            {Http::Code::TemporarilyUnavailable, k503},
            {Http::Code::GatewayTimeout, k504},
            {Http::Code::HttpVersionNotSupported, k505},
            {Http::Code::InsufficientStorage, k507}
    };
    auto it = error_pages.find(code);
    if (it != error_pages.end()) {
        return it->second;
    }
    throw std::logic_error("Unknown http error code");
}

std::string GetErrorPageByCode(Http::Code code, const std::shared_ptr<ServerConfig>& server_config) {
    auto it = server_config->error_pages.find(code);

    if (it != server_config->error_pages.end()) {
        std::string error_page;
        if (ReadFile(it->second, error_page)) {
            return error_page;
        }
    }

    return GetHttpErrorPageByCode(code);
}

}