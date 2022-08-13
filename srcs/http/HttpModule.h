#pragma once

#include "ServerInstance.h"
#include "Connection.h"
#include "Request.h"
#include "logging.h"

#include <unordered_map>

class HttpModule {
public:
    static Request ProcessParseHttpRequest(std::shared_ptr<Connection> connection, std::shared_ptr<std::string> raw_part_request) {
        LOG_INFO("Process ParseHttpRequest");
        Request request;
        request.headers.content_length = 2;
    }

    static Request& GetRequest(int32_t socket_fd) {
        auto it = _http_request_handlers.find(socket_fd);
        if (it != _http_request_handlers.end()) {
            return it->second;
        }

        Request request;
        _http_request_handlers.emplace(socket_fd, request);
        return _http_request_handlers[socket_fd];
    }

private:
    static std::unordered_map<int32_t, Request> _http_request_handlers;
};
