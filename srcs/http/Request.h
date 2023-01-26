#pragma once

#include "SharedPtr.h"
#include "Optional.h"
#include "Config.h"
#include "Http.h"

#include <string>
#include <unordered_map>
#include <vector>

typedef std::unordered_map<std::string, std::vector<std::string> >::iterator HeaderIterator;

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

    bool operator==(const HttpVersion& other) const {
        return major == other.major && minor == other.minor;
    }

    bool operator!=(const HttpVersion& other) const {
        return !(*this == other);
    }

    int major;
    int minor;
};

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
    Request();

    Request(SharedPtr<ServerConfig> default_server_config);

    RequestHandleStatus::Status Handle(SharedPtr<std::string> raw_request_part);

    void Process(const SharedPtr<PortServersConfig>& port_servers_config);

    void AddHeader(const std::string& key, const std::string& value);

    void Clear();

public:
    std::string raw_method;

    Http::Method method;

    HttpVersion raw_http_version;

    Http::Version http_version;

    std::string body;

    /// TODO limit size on this and on headers and on body
    std::string raw;

    std::unordered_map<std::string, std::vector<std::string> > headers;

    RequestTarget target;

    Optional<size_t> content_length;

    bool is_cgi;

    bool keep_alive;

    int keep_alive_timeout;

    SharedPtr<ServerConfig> server_config; /// it could change during ProcessHostHeader

    SharedPtr<Location> location;


private: /// handle helpers
    RequestHandleState::State ParseFirstLineHandler();

    RequestHandleState::State ParseHeaderHandler();

    RequestHandleState::State ParseChunkSizeHandler();

    RequestHandleState::State ParseChunkBodyHandler();

    RequestHandleState::State ParseChunkTrailerPartHandler();

    RequestHandleState::State ParseBodyByContentLengthHandler();

    RequestHandleState::State AnalyzeHeadersBeforeParseBodyHandler();


    RequestHandleState::State _handle_state;

    size_t _handled_size;

    size_t _chunk_body_size;

private: /// process helpers
    void ProcessHttpVersion();

    void ProcessHostHeader(SharedPtr<PortServersConfig> port_servers_config);

    void ProcessContentTypeHeader();

    void ProcessContentLengthHeader();

    void ProcessConnectionHeader();

    void ProcessKeepAliveHeader();

    void ProcessAcceptHeader();

    void ProcessAcceptEncodingHeader();

    void ProcessCookiesHeader();

    void ProcessRouteLocation();

    void ProcessAuthorizationHeader();

    void ProcessMethod();
};
