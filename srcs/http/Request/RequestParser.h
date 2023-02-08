#pragma once

#include "Request.h"

#include <optional>

enum class RequestParseStatus {
    Finish = 0,
    WaitMoreData,
};

enum class RequestParseState {
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

struct RequestParseResult {
    RequestParseStatus status;
    std::optional<std::shared_ptr<Request>> request;
};

class RequestParser {
private:
    std::shared_ptr<EndpointConfig> _endpoint_config;

    std::optional<std::shared_ptr<ServerConfig>> _matched_server_config;

    RequestParseState _parse_state;

    size_t _parsed_size;

    size_t _chunk_body_size;

    /// TODO limit size on this and on _http_headers and on body
    std::string _raw_request;

    Http::Method _http_method;

    Http::Version _http_version;

    std::string _body;

    std::unordered_map<std::string, std::vector<std::string>> _http_headers;

    RequestTarget _target;

    std::optional<size_t> _content_length;

public:
    RequestParser(const std::shared_ptr<EndpointConfig>& endpoint_config);

    RequestParseResult Parse(std::string_view raw_request_part);

private:
    RequestParseState ParseFirstLineHandler();

    RequestParseState ParseHeaderHandler();

    RequestParseState ParseChunkSizeHandler();

    RequestParseState ParseChunkBodyHandler();

    RequestParseState ParseChunkTrailerPartHandler();

    RequestParseState ParseBodyByContentLengthHandler();

    RequestParseState AnalyzeHeadersBeforeParseBodyHandler();

    void AddHeader(const std::string& key, const std::string& value);

    void SetMatchedServerConfig(const std::string& server_name);

    std::shared_ptr<ServerConfig> GetServerConfig();

    void Reset();
};

