#include "RequestParser.h"
#include "Exception.h"
#include "ErrorPages.h"
#include "utilities.h"
#include "Logging.h"

namespace {
    Http::Method ParseHttpMethod(const std::string& raw_method, const std::shared_ptr<ServerConfig>& server_config) {
        if (!IsTcharString(raw_method)) {
            throw Http::BadFirstLine("Incorrect first line", server_config);
        }
        return Http::ToHttpMethod(raw_method);
    }

    Http::RequestTarget
    ParseRequestTarget(const std::string& raw_request_target, const std::shared_ptr<ServerConfig>& server_config) {
        Http::RequestTarget request_target;

        size_t path_end = raw_request_target.find_first_of('?');

        if (path_end == std::string::npos) {
            request_target.path = raw_request_target;
            request_target.directory_path = raw_request_target.substr(0, raw_request_target.find_last_of('/') + 1);
            request_target.query_string = "";
        }
        else {
            request_target.path = raw_request_target.substr(0, path_end);
            request_target.directory_path = request_target.path
                    .substr(0, request_target.path.find_last_of('/') + 1);
            request_target.query_string = raw_request_target.substr(path_end + 1);
        }

        if (request_target.path != request_target.directory_path) {
            request_target.file_name = request_target.path.substr(request_target.path.find_last_of('/') + 1);
            if (request_target.file_name.find('.') != std::string::npos) {
                request_target.extension = request_target.file_name.substr(
                        request_target.file_name.find_last_of('.') + 1);
            }
        }
        else {
            request_target.file_name = "";
            request_target.extension = "";
        }


        if (!IsAbsolutePath(request_target.path)) {
            throw Http::BadFirstLine("Incorrect first line", server_config);
        }
        if (!IsQueryString(request_target.query_string)) {
            throw Http::BadFirstLine("Incorrect first line", server_config);
        }

        return request_target;
    }

    Http::Version ParseHttpVersion(const std::string& raw_http_version,
                                   const std::shared_ptr<ServerConfig>& server_config) {
        std::vector<std::string> tokens = SplitString(raw_http_version, "/");
        if (tokens.size() != 2 || tokens[0] != "HTTP") {
            throw Http::BadHttpVersion("Incorrect HTTP version", server_config);
        }

        std::vector<std::string> version_tokens = SplitString(tokens[1], ".");
        if (version_tokens.size() != 2) {
            throw Http::BadHttpVersion("Incorrect HTTP version", server_config);
        }

        int major, minor;
        try {
            major = ParsePositiveInt(version_tokens[0]);
            minor = ParsePositiveInt(version_tokens[1]);
        }
        catch (const std::exception& e) {
            throw Http::BadHttpVersion("Incorrect HTTP version" + std::string(e.what()), server_config);
        }

        if (major == 1 && minor == 0) { return Http::Http1_0; }
        else if (major == 1 && minor == 1) { return Http::Http1_1; }
        else { throw Http::VersionNotSupported("HTTP version not supported", server_config); }
    }

}

namespace Http {

RequestParseState RequestParser::ParseBodyByContentLengthHandler() {
    /// need _raw_request size without CRLF
    if (_raw_request.size() - _parsed_size < *_content_length) {
        return RequestParseState::WaitData;
    }
    else {
        _body += _raw_request.substr(_parsed_size, *_content_length);
        if (_body.size() > static_cast<size_t>(GetServerConfig()->max_body_size)) {
            throw PayloadTooLarge("Body too large", GetServerConfig());
        }

        _parsed_size += *_content_length;
        /// for now _parsed_size could be lower than _raw_request size if there are some spam after _content_length
        return RequestParseState::FinishHandle;
    }
}

RequestParseState RequestParser::ParseChunkTrailerPartHandler() {
    size_t trailer_end = _raw_request.find(CRLF, _parsed_size);
    if (trailer_end == std::string::npos) {
        return RequestParseState::WaitData;
    }
    /// for now ignore chunked trailer part data
    _content_length = _body.size();
    _parsed_size = trailer_end + CRLF_LEN;
    /// _parsed_size could be lower than _raw_request size if there are some spam after chunked trailer part.
    return RequestParseState::FinishHandle;
}

RequestParseState RequestParser::ParseChunkSizeHandler() {
    size_t chunk_size_end = _raw_request.find(CRLF, _parsed_size);

    if (chunk_size_end == std::string::npos) {
        return RequestParseState::WaitData;
    }

    std::vector<std::string> tokens = SplitString(_raw_request.substr(_parsed_size, chunk_size_end - _parsed_size),
                                                  DELIMITERS);
    if (tokens.empty()) {
        throw BadChunkSize("Incorrect chunk size", GetServerConfig());
    }

    try {
        _chunk_body_size = ParsePositiveInt(tokens[0], 16);
        _parsed_size = chunk_size_end + CRLF_LEN;
        /// for now just ignore chunk extensions
    }
    catch (const std::exception& e) {
        throw BadChunkSize("Incorrect chunk size" + std::string(e.what()), GetServerConfig());
    }

    if (_chunk_body_size == 0) {
        return RequestParseState::HandleChunkTrailerPart;
    }
    return RequestParseState::HandleChunkBody;
}

RequestParseState RequestParser::ParseChunkBodyHandler() {
    size_t raw_size_without_CRLF = _raw_request.size() - CRLF_LEN;
    if (raw_size_without_CRLF < _parsed_size + _chunk_body_size) {
        return RequestParseState::WaitData;
    }

    if (_raw_request.substr(_parsed_size + _chunk_body_size, CRLF_LEN) != CRLF) {
        throw BadChunkBody("Incorrect chunk body", GetServerConfig());
    }

    _body += _raw_request.substr(_parsed_size, _chunk_body_size);
    if (_body.size() > static_cast<size_t>(GetServerConfig()->max_body_size)) {
        throw PayloadTooLarge("Body too large", GetServerConfig());
    }

    _parsed_size += _chunk_body_size + CRLF_LEN;
    return RequestParseState::HandleChunkSize;
}

RequestParseState RequestParser::AnalyzeHeadersBeforeParseBodyHandler() {
    /// transfer encoding
    auto it = _http_headers.find(TRANSFER_ENCODING);
    if (it != _http_headers.end()) {
        if (it->second.size() != 1) {
            throw BadHeader("Incorrect header", GetServerConfig());
        }
        std::vector<std::string> tokens = SplitString(it->second.front(), DELIMITERS);
        if (tokens.size() != 1 || ToLower(StripString(tokens[0])) != CHUNKED) {
            throw UnsupportedTransferEncoding("Unsupported transfer encoding: " + it->second.front(),
                                              GetServerConfig());
        }
        return RequestParseState::HandleChunkSize;
    }

    /// content length
    it = _http_headers.find(CONTENT_LENGTH);
    if (it != _http_headers.end()) {
        if (it->second.size() != 1) {
            throw BadHeader("Incorrect header", GetServerConfig());
        }
        std::vector<std::string> tokens = SplitString(it->second.front(), DELIMITERS);

        if (tokens.size() != 1) {
            throw BadContentLength("Incorrect content length", GetServerConfig());
        }

        try {
            _content_length = ParsePositiveInt(tokens[0]);
            return RequestParseState::HandleBodyByContentLength;
        }
        catch (const std::exception& e) {
            throw BadContentLength("Incorrect content length" + std::string(e.what()), GetServerConfig());
        }
    }

    /// if there are no transfer-encoding chunked and content-length _http_headers, then we finish parse and consider body as empty
    return RequestParseState::FinishHandle;
}

RequestParseState RequestParser::ParseHeaderHandler() {
    size_t header_end = _raw_request.find(CRLF, _parsed_size);

    if (header_end == std::string::npos) {
        return RequestParseState::WaitData;
    }
    else if (header_end == _parsed_size) {
        /// two empty lines in a _raw_request, switch to body handle
        _parsed_size += CRLF_LEN;
        return RequestParseState::AnalyzeBodyHeaders;
    }

    size_t key_end = FindInRange(_raw_request, ":", _parsed_size, header_end);
    if (key_end == std::string::npos) {
        throw BadHeader("Incorrect header", GetServerConfig());
    }
    std::string key = _raw_request.substr(_parsed_size, key_end - _parsed_size);
    if (key.empty() || !IsTcharString(key)) {
        throw BadHeader("Incorrect header", GetServerConfig());
    }
    std::string value = _raw_request.substr(key_end + 1, header_end - key_end - 1);
    if (value.empty() || !IsFieldContent(value)) {
        throw BadHeader("Incorrect header", GetServerConfig());
    }

    AddHeader(key, value);

    _parsed_size = header_end + CRLF_LEN;

    return RequestParseState::HandleHeader;
}

RequestParseState RequestParser::ParseFirstLineHandler() {
    size_t first_line_end = _raw_request.find(CRLF, _parsed_size);

    if (first_line_end == std::string::npos) {
        return RequestParseState::WaitData;
    }
    else if (first_line_end == _parsed_size) {
        /// skip empty lines
        _parsed_size += CRLF_LEN;
        return RequestParseState::HandleFirstLine;
    }

    std::vector<std::string> tokens = SplitString(_raw_request.substr(_parsed_size, first_line_end - _parsed_size),
                                                  DELIMITERS);
    if (tokens.size() != 3) { throw BadFirstLine("Incorrect first line", GetServerConfig()); }

    _http_method = ParseHttpMethod(tokens[0], GetServerConfig());
    _target = ParseRequestTarget(tokens[1], GetServerConfig());
    _http_version = ParseHttpVersion(tokens[2], GetServerConfig());
    _parsed_size = first_line_end + CRLF_LEN;
    return RequestParseState::HandleHeader;
}

RequestParseResult RequestParser::Parse(std::string_view raw_request_part) {
    _raw_request += raw_request_part;
    if (_raw_request.size() > static_cast<size_t>(GetServerConfig()->max_request_size)) {
        throw PayloadTooLarge("Payload too large", GetServerConfig());
    }
    RequestParseState prev_state = _parse_state;

    while (true) {
        switch (_parse_state) {
            case RequestParseState::HandleFirstLine:
                prev_state = _parse_state;
                _parse_state = ParseFirstLineHandler();
                LOG_DEBUG("ParseFirstLine State");
                break;

            case RequestParseState::HandleHeader:
                prev_state = _parse_state;
                _parse_state = ParseHeaderHandler();
                LOG_DEBUG("ParseHeader State");
                break;

            case RequestParseState::AnalyzeBodyHeaders:
                prev_state = _parse_state;
                _parse_state = AnalyzeHeadersBeforeParseBodyHandler();
                LOG_DEBUG("AnalyzeBodyHeaders State");
                break;

            case RequestParseState::HandleChunkSize:
                prev_state = _parse_state;
                _parse_state = ParseChunkSizeHandler();
                LOG_DEBUG("ParseChunkSize State");
                break;

            case RequestParseState::HandleChunkBody:
                prev_state = _parse_state;
                _parse_state = ParseChunkBodyHandler();
                LOG_DEBUG("ParseChunkBody State");
                break;

            case RequestParseState::HandleChunkTrailerPart:
                prev_state = _parse_state;
                _parse_state = ParseChunkTrailerPartHandler();
                LOG_DEBUG("ParseChunkTrailerPart State");
                break;

            case RequestParseState::HandleBodyByContentLength:
                prev_state = _parse_state;
                _parse_state = ParseBodyByContentLengthHandler();
                LOG_DEBUG("ParseBodyByContentLength State");
                break;

            case RequestParseState::WaitData:
                _parse_state = prev_state;
                LOG_DEBUG("WaitData State");
                return {RequestParseStatus::WaitMoreData, std::nullopt};

            case RequestParseState::FinishHandle:
                LOG_DEBUG("FinishHandle State");
                std::shared_ptr<Request> request = std::make_shared<Request>(
                        GetServerConfig(),
                        _http_method,
                        _http_version,
                        std::move(_body),
                        std::move(_raw_request),
                        std::move(_http_headers),
                        _content_length,
                        std::move(_target));
                Reset();
                return {RequestParseStatus::Finish, request};
        }
    }
}

RequestParser::RequestParser(const std::shared_ptr<EndpointConfig>& endpoint_config)
        : _endpoint_config(endpoint_config) {}

void RequestParser::AddHeader(const std::string& key, const std::string& value) {
    std::string lower_key = ToLower(key);
    auto it = _http_headers.find(lower_key);
    if (it == _http_headers.end()) {
        _http_headers[lower_key] = std::vector<std::string>();
    }
    std::string handled_value = StripString(value);
    /// We need to get the correct host as early as possible in order to correctly handle errors and parse the body.
    if (lower_key == HOST) {
        /// host:port doesn't support.
        SetMatchedServerConfig(handled_value);
    }

    _http_headers[lower_key].emplace_back(std::move(handled_value));

}

void RequestParser::Reset() {
    /// Note: _matched_server_config, _endpoint_config aren't cleared.

    _parse_state = RequestParseState::HandleFirstLine;
    _parsed_size = 0;
    _chunk_body_size = 0;
    _raw_request.clear();
    _http_method = {};
    _http_version = {};
    _body.clear();
    _http_headers.clear();
    _target.Clear();
    _content_length = std::nullopt;
}

void RequestParser::SetMatchedServerConfig(const std::string& server_name) {
    _matched_server_config = _endpoint_config->GetServerByNameOrDefault(server_name);
}

std::shared_ptr<ServerConfig> RequestParser::GetServerConfig() {
    return _matched_server_config.value_or(_endpoint_config->GetDefaultServer());
}

}