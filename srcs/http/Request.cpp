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
            request_target.directory_path = request_target.full_path.substr(0,
                                                                            request_target.full_path.find_last_of('/') +
                                                                            1);
            request_target.query_string = raw_request_target.substr(path_end + 1);
        }

        if (request_target.full_path != request_target.directory_path) {
            request_target.file_name = request_target.full_path.substr(request_target.full_path.find_last_of('/') + 1);
            if (request_target.file_name.find('.') != std::string::npos) {
                request_target.extension = request_target.file_name.substr(
                        request_target.file_name.find_last_of('.') + 1);
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
          content_length(), /// it should be empty cause it's optional
          is_cgi(false),
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

    raw_http_version = ParseHttpVersion(tokens[2], server_config);

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

void Request::ProcessHttpVersion() {
    if (raw_http_version.major == 1 && raw_http_version.minor == 0) {
        http_version = Http::Http1_0;
    }
    else if (raw_http_version.major == 1 && raw_http_version.minor == 1) {
        http_version = Http::Http1_1;
    }
    else {
        throw HttpVersionNotSupported("HTTP version not supported", server_config);
    }
}

void Request::ProcessFilePath() {
    target.full_path = server_config->root_path + target.full_path;
    target.directory_path = server_config->root_path + target.directory_path;

    if (server_config->cgi_file_extensions.find(target.extension) !=
        server_config->cgi_file_extensions.end()) {
        is_cgi = true;
    }
    else {
        if (target.full_path == target.directory_path) {
            target.full_path += server_config->default_file_name;
        }
        is_cgi = false;
    }
}


void Request::ProcessHostHeader() {
    HeaderIterator it = headers.find(HOST);

    if (it == headers.end()) {
        switch (http_version) {
            case Http::Http1_0:
                break;
            case Http::Http1_1:
                throw BadRequest("Host header is required", server_config);
        }
    }

    const std::string& host_port = it->second.back();
    if (host_port.empty()) {
        throw BadRequest("Host header incorrect", server_config);
    }

    /// Only ipv4 supported

    size_t port_delimiter_pos = host_port.find(':');

    if (port_delimiter_pos != std::string::npos) {
        std::string port = host_port.substr(host_port.find(':') + 1);
        if (!IsPositiveNumberString(port)) {
            throw BadRequest("Host header incorrect", server_config);
        }
    }
    else {
        port_delimiter_pos = host_port.size();
    }

    std::string host = host_port.substr(0, port_delimiter_pos);

    if (!IsIpv4(host) && !IsRegName(host)) {
        throw BadRequest("Host header incorrect", server_config);
    }
}

void Request::ProcessConnectionHeader() {
    HeaderIterator it = headers.find(CONNECTION);

    if (it != headers.end()) {
        if (ToLower(it->second.back()) == KEEP_ALIVE) {
            keep_alive = true;
        }
        else if (ToLower(it->second.back()) == CLOSE) {
            keep_alive = false;
        }
        return;
    }

    switch (http_version) {
        case Http::Http1_0:
            keep_alive = false;
            break;
        case Http::Http1_1:
            keep_alive = true;
            break;
    }
}

void Request::ProcessKeepAliveHeader() {
    keep_alive_timeout = server_config->default_keep_alive_timeout_s;  /// default value

    HeaderIterator it = headers.find(KEEP_ALIVE);
    if (it == headers.end()) {
        return;
    }

    const std::string& value = it->second.back();

    std::vector<std::string> tokens = SplitString(value, ",");

    for (size_t i = 0; i < tokens.size(); ++i) {
        std::string parameter = StripString(tokens[i]);
        std::vector<std::string> parameter_tokens = SplitString(parameter, "=");

        if (parameter_tokens.size() == 2) {
            if (ToLower(parameter_tokens[0]) == TIMEOUT && IsPositiveNumberString(parameter_tokens[1])) {
                keep_alive_timeout = std::min(server_config->max_keep_alive_timeout_s,
                                              ParsePositiveInt(parameter_tokens[1]));
            }
        }
        /// MAX parameter is outdated and not supported
    }
}

void Request::ProcessAcceptHeader() {
    /// Not using by server, may be used by cgi.
}

void Request::ProcessAcceptEncodingHeader() {
    /// Not using by server, may be used by cgi.
}

void Request::ProcessContentTypeHeader() {
    /// Not using by server, may be used by cgi. By default response return "Content-Type: text/html"
}

void Request::ProcessContentLengthHeader() {
    /// No need to process, because content length is already parsed
}

void Request::ProcessCookiesHeader() {
    /// TODO cookies
}

void Request::ProcessAuthorizationHeader() {
    /// TODO authorization
}

void Request::ProcessMethod() {
    method = Http::GetMethod(raw_method);
    switch (method) {
        case Http::Method::GET:
        case Http::Method::HEAD:
            break;


        case Http::Method::POST: {
            if (!content_length.HasValue()) {
                throw LengthRequired("Length required", server_config);
            }
            if (!is_cgi) {
                throw MethodNotAllowed("Method not allowed", server_config);
            }
            break;
        }

        case Http::Method::DELETE: {
            if (!is_cgi) {
                throw MethodNotAllowed("Method not allowed", server_config);
            }
        }

        case Http::Method::PUT:
        case Http::Method::OPTIONS:
        case Http::Method::CONNECT:
        case Http::Method::TRACE:
        case Http::Method::PATCH:
            throw NotImplemented("Method " + raw_method + " is not implemented", server_config);

        case Http::Method::UNKNOWN:
            throw MethodNotAllowed("Method not allowed", server_config);
    }
}


void Request::Process() {
    /// never change order of these methods
    ProcessHttpVersion();
    ProcessFilePath();
    ProcessContentTypeHeader();
    ProcessContentLengthHeader();
    ProcessHostHeader();
    ProcessConnectionHeader();
    ProcessKeepAliveHeader();
    ProcessAcceptHeader();
    ProcessAcceptEncodingHeader();
    ProcessCookiesHeader();
    ProcessMethod();
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
    raw_http_version = HttpVersion();
    body.clear();
    raw.clear();
    headers.clear();
    target.Clear();
    content_length = Optional<size_t>();
    _handle_state = RequestHandleState::HandleFirstLine;
    _handled_size = 0;
    _chunk_body_size = 0;
}