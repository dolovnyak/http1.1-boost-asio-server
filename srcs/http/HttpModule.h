#pragma once

#include "ServerInstance.h"
#include "Connection.h"
#include "Request.h"
#include "logging.h"


class HttpModule {
public:
    static Request ProcessParseHttpRequest(std::shared_ptr<Connection> connection, std::shared_ptr<std::string> raw_request) {
        LOG_INFO("Process ParseHttpRequest");
        std::ignore = connection;

        Request request;
        request.body = *raw_request;
        return request;
    }
};
