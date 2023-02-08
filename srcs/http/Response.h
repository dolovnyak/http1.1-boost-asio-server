#pragma once

#include "Http.h"
#include "HttpErrorPages.h"

/// У респонса должен быть набор базовых хедеров:
/// Date
/// Server
/// Content-Type
/// Content-Length
/// Connection: (keep-alive или close)

/// При error response все есть внутри кроме имени сервера, его надо пробрасывать.

/// у респонса будет конструктор, который будет принимать статус ответа, тайтл, вектор хедеров и тело.

class Response {
public:
    static std::shared_ptr<Response> MakeErrorResponse(Http::Code error_code, const std::string& error_title,
                                      const std::shared_ptr<ServerConfig>& server_config) {

        std::string body = GetHttpErrorPageByCode(error_code, server_config);
        std::vector<Http::Header> headers = {
                Http::Header("Content-Type", "text/html, charset=utf-8"),
                Http::Header("Content-Length", std::to_string(body.size())),
                Http::Header("Server", server_config->name),
                Http::Header("Date", GetCurrentDateTimeString()),
                Http::Header("Connection", "close")
        };

        return std::make_shared<Response>(Http::Version::Http1_1, error_code, error_title, headers, body);
    }

    static Response MakeOkResponse(Http::Version http_version, const std::string& body,
                                   const std::shared_ptr<ServerConfig>& server_config, bool keep_alive) {
        std::vector<Http::Header> headers = {
                Http::Header("Content-Type", "text/html, charset=utf-8"),
                Http::Header("Content-Length", std::to_string(body.size())),
                Http::Header("Server", server_config->name),
                Http::Header("Date", GetCurrentDateTimeString())
        };

        keep_alive ? headers.emplace_back("Connection", "keep-alive")
                   : headers.emplace_back("Connection", "close");

        return {http_version, Http::Code::Ok, "OK", headers, body};
    }

    Response(Http::Version http_version,
             Http::Code code,
             const std::string& title,
             const std::vector<Http::Header>& headers,
             const std::string& body) {
        response = Http::ToString(http_version) + " " + std::to_string(static_cast<int>(code)) + " " + title + "\r\n";
        for (const auto& header: headers) {
            response += header.key + ": " + header.value + "\r\n";
        }
        response += "\r\n";
        response += body;
    }

    std::string response;
};
