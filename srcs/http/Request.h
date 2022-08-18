#pragma once

#include "SharedPtr.h"
#include "common.h"

#include <string>
#include <unordered_map>

enum RequestHandleStatus {
    FinishWithSuccess = 0,
    FinishWithError,
    WaitMoreData,
};

enum RequestHandleState {
    HandleFirstLine = 0,
    HandleFirstLineWaitData,
    HandleHeader,
    HandleHeaderWaitData,
    HandleBody,
    HandleBodyByChunk,
    HandleBodyByContentLength,
    FinishHandle,
    ErrorHandle
};

class Request {
public:
    Request() : _raw_parsed_size(0) {}

    RequestHandleStatus Handle(SharedPtr<std::string> raw_request_part);

private:
    std::string _method;

    std::string _resource_target;

    int _http_major_version;

    int _http_minor_version;

    std::string _body;

    std::string _raw;

    size_t _raw_parsed_size;

    std::unordered_map<std::string, std::string> _headers;

    RequestHandleState _process_state;

private:
    void AddHeader(const std::string& key, const std::string& value);

    RequestHandleState ParseFirstLineHandler();

    RequestHandleState ParseHeaderHandler();

    RequestHandleState ParseBodyByChunkHandler();

    RequestHandleState ParseBodyByContentLengthHandler();

    RequestHandleState AnalyzeBodyHeadersHandler();
};
