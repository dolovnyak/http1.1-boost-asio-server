#pragma once

#include "Http.h"
#include "ErrorPages.h"

/// У респонса должен быть набор базовых хедеров:
/// Date
/// Server
/// Content-Type
/// Content-Length
/// Connection: (keep-alive или close)

namespace Http {

class Response {
public:
    static std::shared_ptr<Response> MakeErrorResponse(Http::Code error_code, const std::string& title,
                                                       const std::shared_ptr<ServerConfig>& server_config,
                                                       const std::vector<Http::Header>& additional_headers) {
        return MakeResponse(error_code, title, GetErrorPageByCode(error_code, server_config),
                            server_config, additional_headers, false);
    }

    static std::shared_ptr<Response> MakeSuccessGetResponse(const std::string& body,
                                                            const std::shared_ptr<ServerConfig>& server_config,
                                                            bool keep_alive) {
        return MakeResponse(Code::Ok, ToString(Code::Ok), body, server_config, {}, keep_alive);
    }

    static std::shared_ptr<Response> MakeSuccessHeadResponse(const std::shared_ptr<ServerConfig>& server_config,
                                                             bool keep_alive) {
        std::vector<Http::Header> headers = {
                Http::Header("Content-Type", "text/html, charset=utf-8"),
                Http::Header("Server", server_config->name),
                Http::Header("Date", GetCurrentDateTimeString())
        };

        keep_alive ? headers.emplace_back("Connection", "keep-alive")
                   : headers.emplace_back("Connection", "close");

        return std::make_shared<Response>(Http::Version::Http1_1, Code::Ok, ToString(Code::Ok), headers, "");
    }

    static std::shared_ptr<Response> MakeRedirectResponse(Http::Code code,
                                                          const std::shared_ptr<ServerConfig>& server_config,
                                                          std::optional<std::string> redirect,
                                                          bool keep_alive) {
        std::vector<Header> headers;
        if (redirect.has_value()) {
            headers.emplace_back("Location", redirect.value());
        }
        return MakeResponse(code, ToString(code), "", server_config, headers, keep_alive);
    }

    static std::shared_ptr<Response> MakeResponse(Http::Code code, const std::string& title, const std::string& body,
                                                  const std::shared_ptr<ServerConfig>& server_config,
                                                  const std::vector<Http::Header>& additional_headers,
                                                  bool keep_alive) {
        std::vector<Http::Header> headers = {
                Http::Header("Content-Type", "text/html, charset=utf-8"),
                Http::Header("Content-Length", std::to_string(body.size())),
                Http::Header("Server", server_config->name),
                Http::Header("Date", GetCurrentDateTimeString())
        };
        for (auto& header: additional_headers) {
            headers.emplace_back(header);
        }

        keep_alive ? headers.emplace_back("Connection", "keep-alive")
                   : headers.emplace_back("Connection", "close");

        return std::make_shared<Response>(Http::Version::Http1_1, code, title, headers, body);
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
        response += "\r\n";
    }

    std::string response;
};

}