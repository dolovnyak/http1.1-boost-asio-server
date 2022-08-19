#include "Request.h"
#include "utilities.h"
#include "Logging.h"
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
                    std::stoi(version_tokens[0]),
                    std::stoi(version_tokens[1])
            };
            return http_version;
        }
        catch (const std::exception& e) {
            throw HttpException("ParseHttpVersion: " + std::string(e.what()), HttpError::BadRequest);
        }
    }
}

RequestHandleState::State Request::ParseFirstLineHandler() {
    size_t first_line_end = _raw.find(LF);

    if (first_line_end == std::string::npos) {
        return RequestHandleState::HandleFirstLineWaitData; /// first line is not complete
    }
    else if (first_line_end == _raw_parsed_size) {
        ++_raw_parsed_size;
        return RequestHandleState::HandleFirstLine;
    }
    else if (first_line_end == _raw_parsed_size + 1 && _raw[_raw_parsed_size] == CR) {
        _raw_parsed_size += 2;
        return RequestHandleState::HandleFirstLine;
    }

    std::vector<std::string> tokens = SplitString(_raw.substr(_raw_parsed_size, first_line_end), SPACE_DELIMITERS);
    if (tokens.size() != 3) {
        throw HttpException("ParseFirstLineHandler", HttpError::BadRequest);
    }
    _method = tokens[0];

    _resource_target = tokens[1];
    if (_resource_target[0] != '/') {
        throw HttpException("ParseFirstLineHandler", HttpError::BadRequest);
    }

    _http_version = ParseHttpVersion(tokens[2]);

    _raw_parsed_size += first_line_end + 1;

    return RequestHandleState::HandleHeader;
}

RequestHandleState::State Request::ParseHeaderHandler() {
    size_t header_end = _raw.find(LF, _raw_parsed_size);

    if (header_end == std::string::npos) {
        return RequestHandleState::HandleHeaderWaitData;
    }
    else if (header_end == _raw_parsed_size) {
        ++_raw_parsed_size;
        return RequestHandleState::HandleBody;
    }
    else if (header_end == _raw_parsed_size + 1 && _raw[_raw_parsed_size] == CR) {
        _raw_parsed_size += 2;
        return RequestHandleState::HandleBody;
    }

    size_t key_end = _raw.find(":", _raw_parsed_size, header_end - _raw_parsed_size);
    if (key_end == std::string::npos) {
        throw HttpException("ParseHeaderHandler", HttpError::BadRequest);
    }
    std::string key = _raw.substr(_raw_parsed_size, key_end);
    if (key.empty() || key.find_first_of("\t\r ") != std::string::npos) {
        throw HttpException("ParseHeaderHandler", HttpError::BadRequest);
    }
    std::string value = _raw.substr(key_end + 1, header_end - key_end - 1);
    AddHeader(key, value);

    _raw_parsed_size += header_end + 1;

    return RequestHandleState::HandleHeader;
}

RequestHandleState::State Request::AnalyzeBodyHeadersHandler() {

    /// transfer encoding
    HeaderIterator it = _headers.find(TRANSFER_ENCODING);
    if (it != _headers.end()) {
        std::vector<std::string> tokens = SplitString(it->second, SPACE_DELIMITERS);
        if (tokens.size() != 1 || ToLower(StripString(tokens[0])) != CHUNKED) {
            throw HttpException("transfer-encoding: " + it->second, HttpError::NotImplemented);
        }
        return RequestHandleState::HandleBodyByChunk;
    }

    /// content length
    it = _headers.find(CONTENT_LENGTH);
    if (it != _headers.end()) {
        std::vector<std::string> tokens = SplitString(it->second, SPACE_DELIMITERS);

        if (tokens.size() != 1) {
            throw HttpException("content-length: " + it->second,
                                BAD_CONTENT_LENGTH_TITLE,
                                HttpError::BadRequest);
        }

        try {
            _content_length = ParseInt(it->second);
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

RequestHandleState::State Request::ParseBodyByChunkHandler() {
//    return ErrorHandle;
}

RequestHandleState::State Request::ParseBodyByContentLengthHandler() {
//    return ErrorHandle;
}

RequestHandleStatus::Status Request::Handle(SharedPtr<std::string> raw_request_part) {
    _raw += *raw_request_part;

    while (true) {
        if (_raw_parsed_size == _raw.size()) {
            return RequestHandleStatus::WaitMoreData;
        }

        switch (_process_state) {
            case RequestHandleState::HandleFirstLine:
                _process_state = ParseFirstLineHandler();
                break;
            case RequestHandleState::HandleFirstLineWaitData:
                _process_state = RequestHandleState::HandleFirstLine;
                return RequestHandleStatus::WaitMoreData;

            case RequestHandleState::HandleHeader:
                _process_state = ParseHeaderHandler();
                break;
            case RequestHandleState::HandleHeaderWaitData:
                _process_state = RequestHandleState::HandleHeader;
                return RequestHandleStatus::WaitMoreData;

            case RequestHandleState::HandleBody:
                _process_state = AnalyzeBodyHeadersHandler();
                break;
            case RequestHandleState::HandleBodyByChunk:
                _process_state = ParseBodyByChunkHandler();
                break;
            case RequestHandleState::HandleBodyByContentLength:
                _process_state = ParseBodyByContentLengthHandler();
                break;

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
