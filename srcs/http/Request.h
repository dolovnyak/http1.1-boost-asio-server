#pragma once

#include "SharedPtr.h"
#include "ServerInfo.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace RequestHandleStatus {
    enum Status {
        Finish = 0,
        WaitMoreData,
    };
}

namespace RequestHandleState {
    enum State {
        HandleFirstLine = 0,
        HandleHeader,
        AnalyzeBodyHeaders,
        HandleChunkSize,
        HandleChunkBody,
        HandleChunkTrailerPart,
        HandleBodyByContentLength,
        WaitData,
        FinishHandle,
    };
}

struct HttpVersion {
    HttpVersion() : major(0), minor(0) {}
    HttpVersion(int major, int minor) : major(major), minor(minor) {}

    int major;
    int minor;
};

class Request {
public:
    Request(SharedPtr<ServerInfo> server_instance_info);

    RequestHandleStatus::Status Handle(SharedPtr<std::string> raw_request_part);

private:
    void AddHeader(const std::string& key, const std::string& value);

    RequestHandleState::State ParseFirstLineHandler();

    RequestHandleState::State ParseHeaderHandler();

    RequestHandleState::State ParseChunkSizeHandler();

    RequestHandleState::State ParseChunkBodyHandler();

    RequestHandleState::State ParseChunkTrailerPartHandler();

    RequestHandleState::State ParseBodyByContentLengthHandler();

    RequestHandleState::State AnalyzeBodyHeadersHandler();

private:
    std::string _method;

    std::string _resource_target;

    HttpVersion _http_version;

    std::string _body;

    /// TODO limit size on this and on headers and on body
    std::string _raw;

    std::unordered_map<std::string, std::vector<std::string> > _headers;

    SharedPtr<ServerInfo> _server_instance_info;

private: /// handle helpers
    RequestHandleState::State _handle_state;

    size_t _handled_size;

    size_t _content_length;

    size_t _chunk_body_size;
};
