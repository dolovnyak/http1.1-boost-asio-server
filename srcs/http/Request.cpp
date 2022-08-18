#include "Request.h"
#include "utilities.h"
#include "Logging.h"
#include "parse.h"

namespace {
    bool ParseHttpVersion(const std::string& http_version, int& major, int& minor) {
        std::vector<std::string> tokens = SplitString(http_version, '/');
        if (tokens.size() != 2 || tokens[0] != "HTTP") {
            return false;
        }

        std::vector<std::string> version_tokens = SplitString(tokens[1], '.');
        major = ParseInt(version_tokens[0]);
        minor = ParseInt(version_tokens[1]);
        return true;
    }
}

RequestHandleState Request::ParseFirstLineHandler() {
    size_t first_line_end = _raw.find(LF);

    /// processing to big first line or skip empty lines before first line
    if (first_line_end == std::string::npos) {
        return HandleFirstLineWaitData;
    }
    else if (first_line_end == _raw_parsed_size) {
        ++_raw_parsed_size;
        return HandleFirstLine;
    }
    else if (first_line_end == _raw_parsed_size + 1 && _raw[_raw_parsed_size] == CR) {
        _raw_parsed_size += 2;
        return HandleFirstLine;
    }

    std::vector<std::string> tokens = SplitString(_raw.substr(_raw_parsed_size, first_line_end), ' ');
    if (tokens.size() != 3) {
        return ErrorHandle;
    }
    _method = tokens[0];

    if (_resource_target[0] != '/') {
        return ErrorHandle;
    }
    _resource_target = tokens[1];

    if (!ParseHttpVersion(tokens[2], _http_major_version, _http_minor_version)) {
        return ErrorHandle;
    }

    _raw_parsed_size += first_line_end + 1;

    return HandleHeader;
}

RequestHandleState Request::ParseHeaderHandler() {
    size_t header_end = _raw.find(LF, _raw_parsed_size);

    /// processing to big first line or finish parse
    if (header_end == std::string::npos) {
        return HandleHeaderWaitData;
    }
    else if (header_end == _raw_parsed_size) {
        ++_raw_parsed_size;
        return HandleBody;
    }
    else if (header_end == _raw_parsed_size + 1 && _raw[_raw_parsed_size] == CR) {
        _raw_parsed_size += 2;
        return HandleBody;
    }

    size_t key_end = _raw.find(":", _raw_parsed_size, header_end - _raw_parsed_size);
    if (key_end == std::string::npos) {
        return ErrorHandle;
    }
    std::string key = _raw.substr(_raw_parsed_size, key_end);
    if (key.empty() || key.find_first_of("\t\r ") != std::string::npos) {
        return ErrorHandle;
    }
    std::string value = _raw.substr(key_end + 1, header_end - key_end - 1);
    AddHeader(key, value);

    _raw_parsed_size += header_end + 1;

    return HandleHeader;
}

RequestHandleState Request::ParseBodyByChunkHandler() {
    return ErrorHandle;
}

RequestHandleState Request::ParseBodyByContentLengthHandler() {
    return ErrorHandle;
}

RequestHandleState Request::AnalyzeBodyHeadersHandler() {
    /// transfer encoding could be only chunked
    if (_headers.find("Transfer-Encoding") != _headers.end() && _headers.at("Transfer-Encoding") == "chunked") {
        _process_state = HandleBodyByChunk;
    }
    else if (_headers.find(CONTENT_LENGTH) != _headers.end()) {
        _process_state = HandleBodyByContentLength;
    }
    else {
        _process_state = FinishHandle;
    }
}

RequestHandleStatus Request::Handle(SharedPtr<std::string> raw_request_part) {
    _raw += *raw_request_part;

    while (true) {
        try {
            if (_raw_parsed_size == _raw.size()) {
                return WaitMoreData;
            }
            switch (_process_state) {

                case HandleFirstLine:
                    _process_state = ParseFirstLineHandler();
                    break;
                case HandleFirstLineWaitData:
                    _process_state = HandleFirstLine;
                    return WaitMoreData;

                case HandleHeader:
                    _process_state = ParseHeaderHandler();
                    break;
                case HandleHeaderWaitData:
                    _process_state = HandleHeader;
                    return WaitMoreData;

                case HandleBody:
                    _process_state = AnalyzeBodyHeadersHandler();
                    break;
                case HandleBodyByChunk:
                    _process_state = ParseBodyByChunkHandler();
                    break;
                case HandleBodyByContentLength:
                    _process_state = ParseBodyByContentLengthHandler();
                    break;

                case FinishHandle:
                    return FinishWithSuccess;
                case ErrorHandle:
                    return FinishWithError;
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR(e.what());
            return FinishWithError;
        }
    }

    /// парсим первую строку
    /// парсим хедеры в мапку [string ключ, string значение]
    /// после двух CRLF - проверяем есть хедер host, если нет - bad request
    /// в зависимости от

}

void Request::AddHeader(const std::string& key, const std::string& value) {
    _headers[key] = StripString(value);
}
