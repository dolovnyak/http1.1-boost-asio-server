#pragma once

#include "ServerInstance.h"
#include "Connection.h"
#include "Request.h"
#include "logging.h"
#include "HeaderValue.h"

#include <unordered_map>
#include <vector>

enum class ParseRequestStatus {
    Finish = 0,
    WaitMoreData,
};

struct ParseRequestResult {
    ParseRequestStatus status;
    Request request;
};

class HttpModule {
public:
    static ParseRequestResult ParseRequest(std::shared_ptr<std::string> raw_request_part, Request request);

    static Response ProcessRequest(Request request);

    static Response MakeErrorResponse(RequestStatus status);

private:
    static std::unordered_map<std::string, HeaderValue> _builtin_headers;
};

