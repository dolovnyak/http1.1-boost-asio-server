#pragma once

#include "SharedPtr.h"
#include "common.h"

#include <string>
#include <unordered_map>

namespace RequestHandleStatus {
    enum Status {
        FinishWithSuccess = 0,
        WaitMoreData,
    };
}

namespace RequestHandleState {
    enum State {
        HandleFirstLine = 0,
        HandleFirstLineWaitData,
        HandleHeader,
        HandleHeaderWaitData,
        HandleBody,
        HandleBodyByChunk,
        HandleBodyByContentLength,
        FinishHandle,
    };
}

struct HttpVersion {
    int major;
    int minor;
};

class Request {
public:
    Request() : _raw_parsed_size(0) {}

    RequestHandleStatus::Status Handle(SharedPtr<std::string> raw_request_part);

private:
    void AddHeader(const std::string& key, const std::string& value);

    RequestHandleState::State ParseFirstLineHandler();

    RequestHandleState::State ParseHeaderHandler();

    RequestHandleState::State ParseBodyByChunkHandler();

    RequestHandleState::State ParseBodyByContentLengthHandler();

    RequestHandleState::State AnalyzeBodyHeadersHandler();

private:
    std::string _method;

    std::string _resource_target;

    HttpVersion _http_version;

    std::string _body;

    std::string _raw;

    size_t _raw_parsed_size;

    std::unordered_map<std::string, std::string> _headers;

    RequestHandleState::State _process_state;

    int _content_length;
};
