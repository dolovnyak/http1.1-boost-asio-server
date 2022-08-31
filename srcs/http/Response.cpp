#include "Response.h"
#include "utilities.h"

#include <vector>

Response Response::MakeErrorResponse(Http::Code error, const std::string& error_title, SharedPtr<ServerInfo> server_instance_info) {
    std::string body = GetHttpErrorPageByCode(error);
    std::vector<Http::Header> headers;
    headers.push_back(Http::Header("Content-Type", "text/html"));
    headers.push_back(Http::Header("Content-Length", std::to_string(body.size())));
    headers.push_back(Http::Header("Server", server_instance_info->name));
    headers.push_back(Http::Header("Date", GetCurrentDateTime()));
    headers.push_back(Http::Header("Connection", "close"));
    return Response(error, error_title, headers, body);
}

Response Response::MakeOkResponse(const std::string& body, SharedPtr<ServerInfo> server_instance_info) {
    std::vector<Http::Header> headers;
    headers.push_back(Http::Header("Content-Type", "text/html"));
    headers.push_back(Http::Header("Content-Length", std::to_string(body.size())));
    headers.push_back(Http::Header("Server", server_instance_info->name));
    headers.push_back(Http::Header("Date", GetCurrentDateTime()));
    headers.push_back(Http::Header("Connection", "close"));
    return Response(Http::Code::OK, "OK", headers, body);
}

Response::Response(Http::Code code, const std::string& title,
                   const std::vector<Http::Header>& headers, const std::string& body) {
    raw_response = "HTTP/1.1 " + std::to_string(code) + " " + title + "\r\n";
    for (size_t i = 0; i < headers.size(); ++i) {
        raw_response += headers[i].key + ": " + headers[i].value + "\r\n";
    }
    raw_response += "\r\n";
    raw_response += body;
}
