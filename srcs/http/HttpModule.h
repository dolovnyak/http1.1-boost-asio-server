#pragma once

#include "ServerInstance.h"
#include "ConnectionInstance.h"
#include "logging.h"


class HttpModule {
public:
    static void ProcessParseHttpRequest(const ServerInstance& server_instance, const ConnectionInstance& connection_instance, std::shared_ptr<std::string> raw_request) {
        std::ignore = server_instance;
        std::ignore = connection_instance;
        std::ignore = raw_request;
        LOG_INFO("Process ParseHttpRequest");
    }
};
