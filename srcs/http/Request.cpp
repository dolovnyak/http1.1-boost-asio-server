#include "Request.h"
#include "utilities.h"
#include "parse.h"
#include "HttpException.h"

typedef std::unordered_map<std::string, std::string>::iterator HeaderIterator;

namespace {
    HttpVersion ParseHttpVersion(const std::string& raw_http_version) {
        std::vector<std::string> tokens = SplitString(raw_http_version, "/");
        if (tokens.size() != 2 || tokens[0] != "HTTP") {
            throw HttpException("ParseHttpVersion", HttpError::BadRequest);
        }

        std::vector<std::string> version_tokens = SplitString(tokens[1], ".");
        if (version_tokens.size() != 2) {
            throw HttpException("ParseHttpVersion", HttpError::BadRequest);
        }

        try {
            HttpVersion http_version = {
                    ParsePositiveInt(version_tokens[0]),
                    ParsePositiveInt(version_tokens[1])
            };
            return http_version;
        }
        catch (const std::exception& e) {
            throw HttpException("ParseHttpVersion: " + std::string(e.what()), HttpError::BadRequest);
        }
    }
}

RequestHandleState::State Request::ParseFirstLineHandler() {
    size_t first_line_end = _raw.find(CRLF);

    if (first_line_end == std::string::npos) {
        return RequestHandleState::WaitData;
    }
    else if (first_line_end == _handled_size) {
        ++_handled_size;  /// skip empty lines
        return RequestHandleState::HandleFirstLine;
    }

    std::vector<std::string> tokens = SplitString(_raw.substr(_handled_size, first_line_end), DELIMITERS);
    if (tokens.size() != 3) {
        throw HttpException("ParseFirstLineHandler", HttpError::BadRequest);
    }
    _method = tokens[0];

    _resource_target = tokens[1];
    if (_resource_target[0] != '/') {
        throw HttpException("ParseFirstLineHandler", HttpError::BadRequest);
    }

    _http_version = ParseHttpVersion(tokens[2]);

    _handled_size += first_line_end + 1;

    return RequestHandleState::HandleHeader;
}

RequestHandleState::State Request::ParseHeaderHandler() {
    size_t header_end = _raw.find(CRLF, _handled_size);

    if (header_end == std::string::npos) {
        return RequestHandleState::WaitData;
    }
    else if (header_end == _handled_size) {
        ++_handled_size;
        return RequestHandleState::AnalyzeBodyHeaders;
    }

    size_t key_end = _raw.find(":", _handled_size, header_end - _handled_size);
    if (key_end == std::string::npos) {
        throw HttpException("ParseHeaderHandler", HttpError::BadRequest);
    }
    std::string key = _raw.substr(_handled_size, key_end);
    if (key.empty() || key.find_first_of(DELIMITERS) != std::string::npos) {
        throw HttpException("ParseHeaderHandler", HttpError::BadRequest);
    }
    std::string value = _raw.substr(key_end + 1, header_end - key_end - 1);
    AddHeader(key, value);

    _handled_size += header_end + 1;

    return RequestHandleState::HandleHeader;
}

RequestHandleState::State Request::AnalyzeBodyHeadersHandler() {

    /// transfer encoding
    HeaderIterator it = _headers.find(TRANSFER_ENCODING);
    if (it != _headers.end()) {
        std::vector<std::string> tokens = SplitString(it->second, DELIMITERS);
        if (tokens.size() != 1 || ToLower(StripString(tokens[0])) != CHUNKED) {
            throw HttpException("transfer-encoding: " + it->second, HttpError::NotImplemented);
        }
        return RequestHandleState::HandleChunkSize;
    }

    /// content length
    it = _headers.find(CONTENT_LENGTH);
    if (it != _headers.end()) {
        std::vector<std::string> tokens = SplitString(it->second, DELIMITERS);

        if (tokens.size() != 1) {
            throw HttpException("content-length: " + it->second,
                                BAD_CONTENT_LENGTH_TITLE,
                                HttpError::BadRequest);
        }

        try {
            _content_length = ParsePositiveInt(it->second);
            return RequestHandleState::HandleBodyByContentLength;
        }
        catch (const std::exception& e) {
            throw HttpException("content-length: " + it->second + " " + e.what(),
                                BAD_CONTENT_LENGTH_TITLE,
                                HttpError::BadRequest);
        }
    }


    /// if there are no transfer-encoding chunked and content-length headers, then we finish parse and consider body as empty
    return RequestHandleState::FinishHandle;
}

RequestHandleState::State Request::ParseChunkSizeHandler() {
    size_t chunk_size_end = _raw.find(CRLF, _handled_size);

    if (chunk_size_end == std::string::npos) {
        return RequestHandleState::WaitData;
    }

    std::vector<std::string> tokens = SplitString(_raw.substr(_handled_size, chunk_size_end), DELIMITERS);
    if (tokens.empty()) {
        throw HttpException("ParseChunkSizeHandler", "Incorrect chunk size.", HttpError::BadRequest);
    }

    try {
        _chunk_body_size = ParsePositiveInt(tokens[0], 16);
        /// for now just ignore chunk extensions
        _handled_size += chunk_size_end + 1;
    }
    catch (const std::exception& e) {
        throw HttpException("ParseChunkSizeHandler: " + std::string(e.what()), HttpError::BadRequest);
    }

    if (_chunk_body_size == 0) {
        return RequestHandleState::HandleChunkTrailerPart;
    }

    return RequestHandleState::HandleChunkBody;
}

RequestHandleState::State Request::ParseChunkBodyHandler() {
    size_t raw_size_without_CRLF = _raw.size() - 2;
    if (raw_size_without_CRLF - _handled_size < _chunk_body_size) {
        return RequestHandleState::WaitData;
    }

    if (_raw.substr(_handled_size + _chunk_body_size, 2) != CRLF) {
        throw HttpException("ParseChunkBodyHandler", "Incorrect chunk body.", HttpError::BadRequest);
    }

    _body += _raw.substr(_handled_size, _chunk_body_size);
    _handled_size += _chunk_body_size + 2;
    return RequestHandleState::HandleChunkSize;
}

RequestHandleState::State Request::ParseChunkTrailerPartHandler() {
    size_t trailer_end = _raw.find(CRLF, _handled_size);
    if (trailer_end == std::string::npos) {
        return RequestHandleState::WaitData;
    }
    /// for now ignore chunked trailer part data
    _content_length = _body.size();
    return RequestHandleState::FinishHandle;
}

RequestHandleState::State Request::ParseBodyByContentLengthHandler() {
    /// need raw size without CRLF
    if (_raw.size() - _handled_size < _content_length) {
        return RequestHandleState::WaitData;
    }
    else {
        _body += _raw.substr(_handled_size, _content_length);
        _handled_size += _content_length;
        return RequestHandleState::FinishHandle;
    }
}

RequestHandleStatus::Status Request::Handle(SharedPtr<std::string> raw_request_part) {
    _raw += *raw_request_part;
    RequestHandleState::State prev_state = _handle_state;

    while (true) {
        if (_handled_size == _raw.size()) {
            return RequestHandleStatus::WaitMoreData;
        }

        switch (_handle_state) {
            case RequestHandleState::HandleFirstLine:
                prev_state = _handle_state;
                _handle_state = ParseFirstLineHandler();
                break;

            case RequestHandleState::HandleHeader:
                prev_state = _handle_state;
                _handle_state = ParseHeaderHandler();
                break;

            case RequestHandleState::AnalyzeBodyHeaders:
                prev_state = _handle_state;
                _handle_state = AnalyzeBodyHeadersHandler();
                break;

            case RequestHandleState::HandleChunkSize:
                prev_state = _handle_state;
                _handle_state = ParseChunkSizeHandler();
                break;

            case RequestHandleState::HandleChunkBody:
                prev_state = _handle_state;
                _handle_state = ParseChunkBodyHandler();
                break;

            case RequestHandleState::HandleChunkTrailerPart:
                prev_state = _handle_state;
                _handle_state = ParseChunkTrailerPartHandler();
                break;

            case RequestHandleState::HandleBodyByContentLength:
                prev_state = _handle_state;
                _handle_state = ParseBodyByContentLengthHandler();
                break;

            case RequestHandleState::WaitData:
                _handle_state = prev_state;
                return RequestHandleStatus::WaitMoreData;

            case RequestHandleState::FinishHandle:
                return RequestHandleStatus::FinishWithSuccess;
        }
    }
}

void Request::AddHeader(const std::string& key, const std::string& value) {
    std::string lower_key = ToLower(key);
    HeaderIterator it = _headers.find(lower_key);
    if (it != _headers.end()) {
        throw HttpException("Duplicate header: " + key, HttpError::BadRequest);
    }
    _headers[lower_key] = StripString(value);
}
