#pragma once

#include "Config.h"
#include "Http.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Http {

struct RequestTarget {
    std::string path;
    std::string directory_path;
    std::string file_name;
    std::string extension;
    std::string query_string;

    void Clear() {
        path.clear();
        directory_path.clear();
        file_name.clear();
        extension.clear();
        query_string.clear();
    }
};

class Request {
public:
    Request(const std::shared_ptr<ServerConfig>& server_config,
            Http::Method http_method, Http::Version http_version,
            std::string body, std::string raw_request,
            std::unordered_map<std::string, std::vector<std::string>> http_headers,
            const std::optional<size_t>& content_length,
            RequestTarget target)
            : server_config(server_config),
              http_method(http_method),
              http_version(http_version),
              body(std::move(body)), raw_request(std::move(raw_request)),
              http_headers(std::move(http_headers)),
              content_length(content_length),
              target(std::move(target)) {}

    std::shared_ptr<ServerConfig> server_config;

    Http::Method http_method;

    Http::Version http_version;

    std::string body;

    std::string raw_request;

    std::unordered_map<std::string, std::vector<std::string>> http_headers;

    std::optional<size_t> content_length;

    RequestTarget target;
};

}