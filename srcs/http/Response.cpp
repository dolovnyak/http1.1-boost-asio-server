#include "Response.h"
#include "Config.h"
#include "utilities.h"

#include <vector>

Response Response::MakeErrorResponse(Http::Code error, const std::string& error_title,
                                     Optional<SharedPtr<ServerConfig> > server_config) {
    std::string body = GetHttpErrorPageByCode(error);
    std::vector<Http::Header> headers;
    headers.push_back(Http::Header("Content-Type", "text/html, charset=utf-8"));
    headers.push_back(Http::Header("Content-Length", std::to_string(body.size())));
    headers.push_back(Http::Header("Server", WEBSERVER_NAME));
    headers.push_back(Http::Header("Date", GetCurrentDateTimeString()));
    headers.push_back(Http::Header("Connection", "close"));
    return Response(error, error_title, headers, body);
}

Response Response::MakeOkResponse(const std::string& body, Optional<SharedPtr<ServerConfig> > server_config, bool keep_alive) {
    std::vector<Http::Header> headers;
    headers.push_back(Http::Header("Content-Type", "text/html, charset=utf-8"));
    headers.push_back(Http::Header("Content-Length", std::to_string(body.size())));
    headers.push_back(Http::Header("Server", WEBSERVER_NAME));
    headers.push_back(Http::Header("Date", GetCurrentDateTimeString()));

    keep_alive ? headers.push_back(Http::Header("Connection", "keep-alive"))
               : headers.push_back(Http::Header("Connection", "close"));

    return Response(Http::Code::OK, "OK", headers, body);
}

Response::Response(Http::Code code, const std::string& title,
                   const std::vector<Http::Header>& headers, const std::string& body) {
    response = "HTTP/1.1 " + std::to_string(code) + " " + title + "\r\n";
    for (size_t i = 0; i < headers.size(); ++i) {
        response += headers[i].key + ": " + headers[i].value + "\r\n";
    }
    response += "\r\n";
    response += body;
}
