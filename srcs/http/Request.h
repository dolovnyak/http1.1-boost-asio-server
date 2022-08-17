#pragma once

#include "SharedPtr.h"
#include "common.h"

#include <string>
#include <unordered_map>


enum RequestParseStatus {
    FinishWithSuccess = 0,
    FinishWithError,
    WaitMoreData,
};

enum RequestParseState {
    ParseFirstLine = 0,
    ParseHeaders,
    FinishParseHeaders,
    ParseBodyByChunk,
    ParseBodyByContentLength,
    Finish,
    Error
};

class Request {
public:
    RequestParseStatus Parse(SharedPtr<std::string> raw_request_part);

private:
    std::string _body;

    std::string _raw;

    size_t _raw_parsed_size;

    std::unordered_map<std::string, std::string> _headers;

    RequestParseState _parse_state;

private:
    void AddHeader(const std::string& key, const std::string& value);

    RequestParseState ParseFirstLineHandler();

    RequestParseState ParseHeaderHandler();

    RequestParseState ParseBodyByChunkHandler();

    RequestParseState ParseBodyByContentLengthHandler();

    RequestParseState AnalyzeBodyHeadersHandler();
};
