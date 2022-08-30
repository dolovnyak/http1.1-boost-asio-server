#include "Request.h"
#include "utilities.h"
#include "HttpException.h"
#include "HttpErrorPages.h"
#include "Logging.h"
#include "ParserWrapper.h"

typedef std::unordered_map<std::string, std::vector<std::string> >::iterator HeaderIterator;

Request::Request(SharedPtr<ServerInfo> server_instance_info)
        : server_instance_info(server_instance_info),
          _handle_state(RequestHandleState::HandleFirstLine),
          _handled_size(0),
          _content_length(),
          _chunk_body_size(0),
          _parser(*this) {}

Request::Request(const Request& other)
        : _parser(*this) {
    method = other.method;
    target = other.target;
    http_version = other.http_version;
    body = other.body;
    raw = other.raw;
    headers = other.headers;
    server_instance_info = other.server_instance_info;
    _handle_state = other._handle_state;
    _handled_size = other._handled_size;
    _content_length = other._content_length;
    _chunk_body_size = other._chunk_body_size;
    LOG_WARNING("Request copy constructor called, it should be used only for tests");
}

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
    _parser.Parse(raw, _handled_size, first_line_end, yy::ParseState::FirstLine);

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

    _parser.Parse(raw, _handled_size, header_end, yy::ParseState::Header);

    _handled_size = header_end + CRLF_LEN;

    return RequestHandleState::HandleHeader;
}

RequestHandleState::State Request::AnalyzeBodyHeadersHandler() {

    /// transfer encoding
    HeaderIterator it = headers.find(TRANSFER_ENCODING);
    if (it != headers.end()) {
        if (it->second.size() != 1) {
            throw BadHeader("Incorrect header", server_instance_info);
        }
        std::vector<std::string> tokens = SplitString(it->second.front(), DELIMITERS);
        if (tokens.size() != 1 || ToLower(StripString(tokens[0])) != CHUNKED) {
            throw UnsupportedTransferEncoding("Unsupported transfer encoding: " + it->second.front(),
                                              server_instance_info);
        }
        return RequestHandleState::HandleChunkSize;
    }

    /// content length
    it = headers.find(CONTENT_LENGTH);
    if (it != headers.end()) {
        if (it->second.size() != 1) {
            throw BadHeader("Incorrect header", server_instance_info);
        }
        std::vector<std::string> tokens = SplitString(it->second.front(), DELIMITERS);

        if (tokens.size() != 1) {
            throw BadContentLength("Incorrect content length", server_instance_info);
        }

        try {
            _content_length = ParsePositiveInt(tokens[0]);
            return RequestHandleState::HandleBodyByContentLength;
        }
        catch (const std::exception& e) {
            throw BadContentLength("Incorrect content length" + std::string(e.what()), server_instance_info);
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
        throw BadChunkSize("Incorrect chunk size", server_instance_info);
    }

    try {
        _chunk_body_size = ParsePositiveInt(tokens[0], 16);
        _handled_size = chunk_size_end + CRLF_LEN;
        /// for now just ignore chunk extensions
    }
    catch (const std::exception& e) {
        throw BadChunkSize("Incorrect chunk size" + std::string(e.what()), server_instance_info);
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
        throw BadChunkBody("Incorrect chunk body", server_instance_info);
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
    _content_length = body.size();
    _handled_size = trailer_end + CRLF_LEN;
    /// for now _handled_size could be lower than raw size if there are some spam after chunked trailer part
    /// and for now I don't do anything with it, same as with content_length handling.
    return RequestHandleState::FinishHandle;
}

RequestHandleState::State Request::ParseBodyByContentLengthHandler() {
    /// need raw size without CRLF
    if (raw.size() - _handled_size < *_content_length) {
        return RequestHandleState::WaitData;
    }
    else {
        body += raw.substr(_handled_size, *_content_length);
        _handled_size += *_content_length;
        /// for now _handled_size could be lower than raw size if there are some spam after content_length
        return RequestHandleState::FinishHandle;
    }
}

RequestHandleStatus::Status Request::Handle(SharedPtr<std::string> raw_request_part) {
    raw += *raw_request_part;
    if (raw.size() >= 1337) { /// TODO get value from config
        throw PayloadTooLarge("Payload too large", server_instance_info);
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
    HeaderIterator it = headers.find(lower_key);
    if (it == headers.end()) {
        headers[lower_key] = std::vector<std::string>();
    }
    headers[lower_key].push_back(StripString(value));
}
