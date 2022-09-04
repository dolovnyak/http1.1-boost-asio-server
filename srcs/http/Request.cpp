#include "Request.h"
#include "utilities.h"
#include "HttpException.h"
#include "HttpErrorPages.h"
#include "Logging.h"

namespace {
    std::string ParseMethod(const std::string& raw_method,
                            const SharedPtr<ServerConfig>& server_config) {
        if (!IsTcharString(raw_method)) {
            throw BadFirstLine("Incorrect first line", server_config);
        }
        return raw_method;
    }

    RequestTarget ParseRequestTarget(const std::string& raw_request_target,
                                     const SharedPtr<ServerConfig>& server_config) {
        RequestTarget request_target;

        size_t path_end = raw_request_target.find_first_of('?');

        if (path_end == std::string::npos) {
            request_target.full_path = raw_request_target;
            request_target.directory_path = raw_request_target.substr(0, raw_request_target.find_last_of('/') + 1);
            request_target.query_string = "";
        }
        else {
            request_target.full_path = raw_request_target.substr(0, path_end);
            request_target.directory_path = request_target.full_path.substr(0, request_target.full_path.find_last_of('/') + 1);
            request_target.query_string = raw_request_target.substr(path_end + 1);
        }

        if (request_target.full_path != request_target.directory_path) {
            request_target.file_name = request_target.full_path.substr(request_target.full_path.find_last_of('/') + 1);
            if (request_target.file_name.find('.') != std::string::npos) {
                request_target.extension = request_target.file_name.substr(request_target.file_name.find_last_of('.') + 1);
            }
        }
        else {
            request_target.file_name = "";
            request_target.extension = "";
        }


        if (!IsAbsolutePath(request_target.full_path)) {
            throw BadFirstLine("Incorrect first line", server_config);
        }
        if (!IsQueryString(request_target.query_string)) {
            throw BadFirstLine("Incorrect first line", server_config);
        }

        return request_target;
    }

    HttpVersion ParseHttpVersion(const std::string& raw_http_version,
                                 const SharedPtr<ServerConfig>& server_config) {
        std::vector<std::string> tokens = SplitString(raw_http_version, "/");
        if (tokens.size() != 2 || tokens[0] != "HTTP") {
            throw BadHttpVersion("Incorrect HTTP version", server_config);
        }

        std::vector<std::string> version_tokens = SplitString(tokens[1], ".");
        if (version_tokens.size() != 2) {
            throw BadHttpVersion("Incorrect HTTP version", server_config);
        }

        try {
            HttpVersion http_version = HttpVersion(
                    ParsePositiveInt(version_tokens[0]),
                    ParsePositiveInt(version_tokens[1])
            );
            return http_version;
        }
        catch (const std::exception& e) {
            throw BadHttpVersion("Incorrect HTTP version" + std::string(e.what()), server_config);
        }
    }
}

Request::Request(const SharedPtr<ServerConfig>& server_config)
        : server_config(server_config),
          content_length(),
          _handle_state(RequestHandleState::HandleFirstLine),
          _handled_size(0),
          _chunk_body_size(0) {}

RequestHandleState::State Request::ParseFirstLineHandler() {
    size_t first_line_end = raw.find(CRLF, _handled_size);

    if (first_line_end == std::string::npos) {
        return RequestHandleState::WaitData;
    }
    else if (first_line_end == _handled_size) {
        /// skip empty lines
        _handled_size += CRLF_LEN;
        return RequestHandleState::HandleFirstLine;
    }

    std::vector<std::string> tokens = SplitString(raw.substr(_handled_size, first_line_end - _handled_size),
                                                  DELIMITERS);
    if (tokens.size() != 3) {
        throw BadFirstLine("Incorrect first line", server_config);
    }
    raw_method = ParseMethod(tokens[0], server_config);

    target = ParseRequestTarget(tokens[1], server_config);

    http_version = ParseHttpVersion(tokens[2], server_config);

    _handled_size = first_line_end + CRLF_LEN;

    return RequestHandleState::HandleHeader;
}

RequestHandleState::State Request::ParseHeaderHandler() {
    size_t header_end = raw.find(CRLF, _handled_size);

    if (header_end == std::string::npos) {
        return RequestHandleState::WaitData;
    }
    else if (header_end == _handled_size) {
        /// two empty lines in a raw, switch to body handle
        _handled_size += CRLF_LEN;
        return RequestHandleState::AnalyzeBodyHeaders;
    }

    size_t key_end = FindInRange(raw, ":", _handled_size, header_end);
    if (key_end == std::string::npos) {
        throw BadHeader("Incorrect header", server_config);
    }
    std::string key = raw.substr(_handled_size, key_end - _handled_size);
    if (key.empty() || !IsTcharString(key)) {
        throw BadHeader("Incorrect header", server_config);
    }
    std::string value = raw.substr(key_end + 1, header_end - key_end - 1);
    if (value.empty() || !IsFieldContent(value)) {
        throw BadHeader("Incorrect header", server_config);
    }

    AddHeader(key, value);

    _handled_size = header_end + CRLF_LEN;

    return RequestHandleState::HandleHeader;
}

RequestHandleState::State Request::AnalyzeHeadersBeforeParseBodyHandler() {


    /// transfer encoding
    HeaderIterator it = headers.find(TRANSFER_ENCODING);
    if (it != headers.end()) {
        if (it->second.size() != 1) {
            throw BadHeader("Incorrect header", server_config);
        }
        std::vector<std::string> tokens = SplitString(it->second.front(), DELIMITERS);
        if (tokens.size() != 1 || ToLower(StripString(tokens[0])) != CHUNKED) {
            throw UnsupportedTransferEncoding("Unsupported transfer encoding: " + it->second.front(),
                                              server_config);
        }
        return RequestHandleState::HandleChunkSize;
    }

    /// content length
    it = headers.find(CONTENT_LENGTH);
    if (it != headers.end()) {
        if (it->second.size() != 1) {
            throw BadHeader("Incorrect header", server_config);
        }
        std::vector<std::string> tokens = SplitString(it->second.front(), DELIMITERS);

        if (tokens.size() != 1) {
            throw BadContentLength("Incorrect content length", server_config);
        }

        try {
            content_length = ParsePositiveInt(tokens[0]);
            return RequestHandleState::HandleBodyByContentLength;
        }
        catch (const std::exception& e) {
            throw BadContentLength("Incorrect content length" + std::string(e.what()), server_config);
        }
    }

    /// if there are no transfer-encoding chunked and content-length headers, then we finish parse and consider body as empty
    return RequestHandleState::FinishHandle;
}

RequestHandleState::State Request::ParseChunkSizeHandler() {
    size_t chunk_size_end = raw.find(CRLF, _handled_size);

    if (chunk_size_end == std::string::npos) {
        return RequestHandleState::WaitData;
    }

    std::vector<std::string> tokens = SplitString(raw.substr(_handled_size, chunk_size_end - _handled_size),
                                                  DELIMITERS);
    if (tokens.empty()) {
        throw BadChunkSize("Incorrect chunk size", server_config);
    }

    try {
        _chunk_body_size = ParsePositiveInt(tokens[0], 16);
        _handled_size = chunk_size_end + CRLF_LEN;
        /// for now just ignore chunk extensions
    }
    catch (const std::exception& e) {
        throw BadChunkSize("Incorrect chunk size" + std::string(e.what()), server_config);
    }

    if (_chunk_body_size == 0) {
        return RequestHandleState::HandleChunkTrailerPart;
    }
    return RequestHandleState::HandleChunkBody;
}

RequestHandleState::State Request::ParseChunkBodyHandler() {
    size_t raw_size_without_CRLF = raw.size() - CRLF_LEN;
    if (raw_size_without_CRLF < _handled_size + _chunk_body_size) {
        return RequestHandleState::WaitData;
    }

    if (raw.substr(_handled_size + _chunk_body_size, CRLF_LEN) != CRLF) {
        throw BadChunkBody("Incorrect chunk body", server_config);
    }

    body += raw.substr(_handled_size, _chunk_body_size);
    _handled_size += _chunk_body_size + CRLF_LEN;
    return RequestHandleState::HandleChunkSize;
}

RequestHandleState::State Request::ParseChunkTrailerPartHandler() {
    size_t trailer_end = raw.find(CRLF, _handled_size);
    if (trailer_end == std::string::npos) {
        return RequestHandleState::WaitData;
    }
    /// for now ignore chunked trailer part data
    content_length = body.size();
    _handled_size = trailer_end + CRLF_LEN;
    /// for now _handled_size could be lower than raw size if there are some spam after chunked trailer part
    /// and for now I don't do anything with it, same as with content_length handling.
    return RequestHandleState::FinishHandle;
}

RequestHandleState::State Request::ParseBodyByContentLengthHandler() {
    /// need raw size without CRLF
    if (raw.size() - _handled_size < *content_length) {
        return RequestHandleState::WaitData;
    }
    else {
        body += raw.substr(_handled_size, *content_length);
        _handled_size += *content_length;
        /// for now _handled_size could be lower than raw size if there are some spam after content_length
        return RequestHandleState::FinishHandle;
    }
}

RequestHandleStatus::Status Request::Handle(SharedPtr<std::string> raw_request_part) {
    raw += *raw_request_part;
    if (raw.size() >= 1337) { /// TODO get value from config
        throw PayloadTooLarge("Payload too large", server_config);
    }
    RequestHandleState::State prev_state = _handle_state;

    while (true) {
        switch (_handle_state) {
            case RequestHandleState::HandleFirstLine:
                prev_state = _handle_state;
                _handle_state = ParseFirstLineHandler();
                LOG_DEBUG("ParseFirstLine State");
                break;

            case RequestHandleState::HandleHeader:
                prev_state = _handle_state;
                _handle_state = ParseHeaderHandler();
                LOG_DEBUG("ParseHeader State");
                break;

            case RequestHandleState::AnalyzeBodyHeaders:
                prev_state = _handle_state;
                _handle_state = AnalyzeHeadersBeforeParseBodyHandler();
                LOG_DEBUG("AnalyzeBodyHeaders State");
                break;

            case RequestHandleState::HandleChunkSize:
                prev_state = _handle_state;
                _handle_state = ParseChunkSizeHandler();
                LOG_DEBUG("ParseChunkSize State");
                break;

            case RequestHandleState::HandleChunkBody:
                prev_state = _handle_state;
                _handle_state = ParseChunkBodyHandler();
                LOG_DEBUG("ParseChunkBody State");
                break;

            case RequestHandleState::HandleChunkTrailerPart:
                prev_state = _handle_state;
                _handle_state = ParseChunkTrailerPartHandler();
                LOG_DEBUG("ParseChunkTrailerPart State");
                break;

            case RequestHandleState::HandleBodyByContentLength:
                prev_state = _handle_state;
                _handle_state = ParseBodyByContentLengthHandler();
                LOG_DEBUG("ParseBodyByContentLength State");
                break;

            case RequestHandleState::WaitData:
                _handle_state = prev_state;
                LOG_DEBUG("WaitData State");
                return RequestHandleStatus::WaitMoreData;

            case RequestHandleState::FinishHandle:
                LOG_DEBUG("FinishHandle State");
                return RequestHandleStatus::Finish;
        }
    }
}

void Request::AddHeader(const std::string& key, const std::string& value) {
    std::string lower_key = ToLower(key);
    HeaderIterator it = headers.find(lower_key);
    if (it == headers.end()) {
        headers[lower_key] = std::vector<std::string>();
    }
    headers[lower_key].push_back(StripString(value));
}

void Request::Clear() {
    raw_method.clear();
    method = Http::Method::UNKNOWN;
    http_version = HttpVersion();
    body.clear();
    raw.clear();
    headers.clear();
    target.Clear();
    content_length = Optional<size_t>();
    _handle_state = RequestHandleState::HandleFirstLine;
    _handled_size = 0;
    _chunk_body_size = 0;
}