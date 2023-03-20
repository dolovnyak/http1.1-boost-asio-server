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


/// default response with body
/// default response without body

class Response {
public:
    static std::vector<Http::Header> GetDefaultHeaders(const std::shared_ptr<ServerConfig>& server_config) {
        return {Header("Server", server_config->name),
                Header("Date", GetCurrentDateTimeString()),
                Header("Content-Type", "text/html, charset=utf-8")};
    }

    static std::shared_ptr<Response> MakeDefaultWithBody(const std::shared_ptr<ServerConfig>& server_config,
                                                         Code code, std::string title, std::string body) {
        std::vector<Header> headers = GetDefaultHeaders(server_config);
        headers.emplace_back("Content-Length", std::to_string(body.size()));

        return std::make_shared<Response>(code, std::move(title), std::move(headers), std::move(body));
    }

    static std::shared_ptr<Response> MakeDefaultWithoutBody(const std::shared_ptr<ServerConfig>& server_config,
                                                            Code code, std::string title) {
        std::vector<Header> headers = GetDefaultHeaders(server_config);
        return std::make_shared<Response>(code, std::move(title), std::move(headers), std::nullopt);
    }

    Response(Http::Code code,
             std::string title,
             std::vector<Http::Header> headers,
             std::optional<std::string> body)
            : _version(Http1_1), _code(code),
              _title(std::move(title)), _headers(std::move(headers)), _body(std::move(body)) {}

    std::string Extract() {
        std::string res =
                Http::ToString(_version) + " " + std::to_string(static_cast<int>(_code)) + " " + _title + "\r\n";
        for (const auto& header: _headers) {
            res += header.key + ": " + header.value + "\r\n";
        }
        res += "\r\n";

        if (_body.has_value() && !_body.value().empty()) {
            res += _body.value();
        }
        return res;
    }

    void AddHeader(Header new_header) {
        _headers.emplace_back(std::move(new_header));
    }

private:
    Version _version;
    Code _code;
    std::string _title;
    std::vector<Header> _headers;
    std::optional<std::string> _body;
};

}