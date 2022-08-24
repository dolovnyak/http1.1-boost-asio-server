#include "Request.h"
#include "utilities.h"
#include "parse.h"
#include "HttpException.h"
#include "HttpErrorPages.h"
#include "Logging.h"

typedef std::unordered_map<std::string, std::vector<std::string> >::iterator HeaderIterator;

namespace {
    HttpVersion ParseHttpVersion(const std::string& raw_http_version) {
        std::vector<std::string> tokens = SplitString(raw_http_version, "/");
        if (tokens.size() != 2 || tokens[0] != "HTTP") {
            throw BadHttpVersion("Incorrect HTTP version");
        }

        std::vector<std::string> version_tokens = SplitString(tokens[1], ".");
        if (version_tokens.size() != 2) {
            throw BadHttpVersion("Incorrect HTTP version");
        }

        try {
            HttpVersion http_version = HttpVersion(
                    ParsePositiveInt(version_tokens[0]),
                    ParsePositiveInt(version_tokens[1])
            );
            return http_version;
        }
        catch (const std::exception& e) {
            throw BadHttpVersion("Incorrect HTTP version" + std::string(e.what()));
        }
    }
}

Request::Request()
        : _handle_state(RequestHandleState::HandleFirstLine),
          _handled_size(0),
          _content_length(0),
          _chunk_body_size(0) {}

RequestHandleState::State Request::ParseFirstLineHandler() {
    size_t first_line_end = _raw.find(kCRLF, _handled_size);

    if (first_line_end == std::string::npos) {
        return RequestHandleState::WaitData;
    }
    else if (first_line_end == _handled_size) {
        _handled_size += kCRLF.size();  /// skip empty lines
        return RequestHandleState::HandleFirstLine;
    }

    std::vector<std::string> tokens = SplitString(_raw.substr(_handled_size, first_line_end - _handled_size),
                                                  DELIMITERS);
    if (tokens.size() != 3) {
        throw BadFirstLine("Incorrect first line");
    }
    _method = tokens[0];

    _resource_target = tokens[1];
    if (_resource_target[0] != '/') {
        throw BadFirstLine("Incorrect first line");
    }

    _http_version = ParseHttpVersion(tokens[2]);

    _handled_size = first_line_end + kCRLF.size();

    return RequestHandleState::HandleHeader;
}

RequestHandleState::State Request::ParseHeaderHandler() {
    size_t header_end = _raw.find(kCRLF, _handled_size);

    if (header_end == std::string::npos) {
        return RequestHandleState::WaitData;
    }
    else if (header_end == _handled_size) {
        _handled_size += kCRLF.size(); /// two empty lines in a raw, switch to body handle
        return RequestHandleState::AnalyzeBodyHeaders;
    }

    size_t key_end = FindInRange(_raw, ":", _handled_size, header_end);
    if (key_end == std::string::npos) {
        throw BadHeader("Incorrect header");
    }
    std::string key = _raw.substr(_handled_size, key_end - _handled_size);
    if (key.empty() || key.find_first_of(DELIMITERS) != std::string::npos) {
        throw BadHeader("Incorrect header");
    }
    std::string value = _raw.substr(key_end + 1, header_end - key_end - 1);
    AddHeader(key, value);

    _handled_size = header_end + kCRLF.size();

    return RequestHandleState::HandleHeader;
}

RequestHandleState::State Request::AnalyzeBodyHeadersHandler() {

    /// transfer encoding
    HeaderIterator it = _headers.find(TRANSFER_ENCODING);
    if (it != _headers.end()) {
        if (it->second.size() != 1) {
            throw BadHeader("Incorrect header");
        }
        std::vector<std::string> tokens = SplitString(it->second.front(), DELIMITERS);
        if (tokens.size() != 1 || ToLower(StripString(tokens[0])) != CHUNKED) {
            throw UnsupportedTransferEncoding("Unsupported transfer encoding: " + it->second.front());
        }
        return RequestHandleState::HandleChunkSize;
    }

    /// content length
    it = _headers.find(CONTENT_LENGTH);
    if (it != _headers.end()) {
        if (it->second.size() != 1) {
            throw BadHeader("Incorrect header");
        }
        std::vector<std::string> tokens = SplitString(it->second.front(), DELIMITERS);

        if (tokens.size() != 1) {
            throw BadContentLength("Incorrect content length");
        }

        try {
            _content_length = ParsePositiveInt(tokens[0]);
            return RequestHandleState::HandleBodyByContentLength;
        }
        catch (const std::exception& e) {
            throw BadContentLength("Incorrect content length" + std::string(e.what()));
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

    std::vector<std::string> tokens = SplitString(_raw.substr(_handled_size, chunk_size_end - _handled_size),
                                                  DELIMITERS);
    if (tokens.empty()) {
        throw BadChunkSize("Incorrect chunk size");
    }

    try {
        _chunk_body_size = ParsePositiveInt(tokens[0], 16);
        _handled_size = chunk_size_end + kCRLF.size();
        /// for now just ignore chunk extensions
    }
    catch (const std::exception& e) {
        throw BadChunkSize("Incorrect chunk size" + std::string(e.what()));
    }

    if (_chunk_body_size == 0) {
        return RequestHandleState::HandleChunkTrailerPart;
    }
    return RequestHandleState::HandleChunkBody;
}

RequestHandleState::State Request::ParseChunkBodyHandler() {
    size_t raw_size_without_CRLF = _raw.size() - kCRLF.size();
    if (raw_size_without_CRLF < _handled_size + _chunk_body_size) {
        return RequestHandleState::WaitData;
    }

    if (_raw.substr(_handled_size + _chunk_body_size, kCRLF.size()) != CRLF) {
        throw BadChunkBody("Incorrect chunk body");
    }

    _body += _raw.substr(_handled_size, _chunk_body_size);
    _handled_size += _chunk_body_size + kCRLF.size();
    return RequestHandleState::HandleChunkSize;
}

RequestHandleState::State Request::ParseChunkTrailerPartHandler() {
    size_t trailer_end = _raw.find(CRLF, _handled_size);
    if (trailer_end == std::string::npos) {
        return RequestHandleState::WaitData;
    }
    /// for now ignore chunked trailer part data
    _content_length = _body.size();
    _handled_size = trailer_end + kCRLF.size();
    /// for now _handled_size could be lower than raw size if there are some spam after chunked trailer part
    /// and for now I don't do anything with it, same as with content_length handling.
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
        /// for now _handled_size could be lower than raw size if there are some spam after content_length
        return RequestHandleState::FinishHandle;
    }
}

RequestHandleStatus::Status Request::Handle(SharedPtr<std::string> raw_request_part) {
    _raw += *raw_request_part;
    if (_raw.size() >= 1337) { /// TODO get value from config
        throw PayloadTooLarge("Payload too large");
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
                _handle_state = AnalyzeBodyHeadersHandler();
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
    HeaderIterator it = _headers.find(lower_key);
    if (it == _headers.end()) {
        _headers[lower_key] = std::vector<std::string>();
    }
    _headers[lower_key].push_back(StripString(value));
}
