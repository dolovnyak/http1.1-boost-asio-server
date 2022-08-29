#pragma once

#include "SharedPtr.h"
#include "ServerInfo.h"
#include "Optional.h"
#include "ParserWrapper.h"

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

struct RequestTarget {
    std::string path;
    std::string query;
};

class Request {
public:
    Request(SharedPtr<ServerInfo> server_instance_info);

    Request(const Request& other);

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

public:
    std::string method;

    std::string resource_target;

    HttpVersion http_version;

    std::string body;

    /// TODO limit size on this and on headers and on body
    std::string raw;

    std::unordered_map<std::string, std::vector<std::string> > headers;

    SharedPtr<ServerInfo> server_instance_info;

    RequestTarget target;

private: /// handle helpers
    RequestHandleState::State _handle_state;

    size_t _handled_size;

    Optional<size_t> _content_length;

    size_t _chunk_body_size;

    ParserWrapper _parser;
};
