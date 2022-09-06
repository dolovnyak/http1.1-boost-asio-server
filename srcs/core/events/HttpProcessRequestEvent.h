#pragma once

#include "Event.h"
#include "HttpException.h"
#include "utilities.h"
#include "HttpFileSession.h"

#include <queue>
#include <unistd.h>
#include <fcntl.h>

template<class CoreModule>
class HttpProcessRequestEvent : public Event {
public:
    HttpProcessRequestEvent(const SharedPtr<Session<CoreModule> >& session,
                            std::queue<SharedPtr<Event> >* event_queue)
            : _packaged_http_session(session), _event_queue(event_queue) {}

    ~HttpProcessRequestEvent() {}

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;


private:
    void ProcessHttpVersion();

    void ProcessHostHeader();

    void ProcessConnectionHeader();

    void ProcessKeepAliveHeader();

    void ProcessFilePath(std::string& file, bool& cgi);

    void ProcessMethod();

    void RunCgiPipeline(const std::string& file);

    void RunFilePipeline(const std::string& file);

    SharedPtr<Session<CoreModule> > _packaged_http_session;

    HttpSession<CoreModule>* _http_session;

    std::queue<SharedPtr<Event> >* _event_queue;
};


template<class CoreModule>
const std::string& HttpProcessRequestEvent<CoreModule>::GetName() const {
    static std::string kName = "HttpProcessRequestEvent";
    return kName;
}

template<class CoreModule>
void HttpProcessRequestEvent<CoreModule>::ProcessHttpVersion() {
    if (_http_session->request->http_version != HttpVersion(1, 1)) {
        throw HttpVersionNotSupported("HTTP version not supported", _http_session->server_config);
    }
}

template<class CoreModule>
void HttpProcessRequestEvent<CoreModule>::ProcessHostHeader() {
    HeaderIterator it = _http_session->request->headers.find(HOST);
    if (it == _http_session->request->headers.end() || it->second.size() != 1) {
        throw BadRequest("Host header incorrect", _http_session->server_config);
    }
}

template<class CoreModule>
void HttpProcessRequestEvent<CoreModule>::ProcessConnectionHeader() {
    HeaderIterator it = _http_session->request->headers.find(CONNECTION);

    if (it != _http_session->request->headers.end()) {
        if (ToLower(it->second.back()) == KEEP_ALIVE) {
            _http_session->keep_alive = true;
        }
        else if (ToLower(it->second.back()) == CLOSE) {
            _http_session->keep_alive = false;
        }
    }
    else {
        _http_session->keep_alive = true; /// default http behavior
    }
}

template<class CoreModule>
void HttpProcessRequestEvent<CoreModule>::ProcessKeepAliveHeader() {
    HeaderIterator it = _http_session->request->headers.find(KEEP_ALIVE);
    if (it == _http_session->request->headers.end()) {
        return;
    }

    const std::string& value = it->second.back();

    std::vector<std::string> tokens = SplitString(value, ",");
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::string parameter = StripString(tokens[i]);
        std::vector<std::string> parameter_tokens = SplitString(parameter, "=");

        if (parameter_tokens.size() == 2) {
            if (ToLower(parameter_tokens[0]) == TIMEOUT && IsPositiveNumberString(parameter_tokens[1])) {
                _http_session->keep_alive_timeout =
                        std::min(_http_session->server_config->max_keep_alive_timeout_s,
                                 std::max(ParsePositiveInt(parameter_tokens[1]),
                                          _http_session->server_config->default_keep_alive_timeout_s));
            }
        }
        /// MAX parameter is outdated and not supported
    }
}

template<class CoreModule>
void HttpProcessRequestEvent<CoreModule>::ProcessFilePath(std::string& file_path, bool& is_cgi) {
    file_path = _http_session->server_config->root_path + _http_session->request->target.full_path;
    std::string directory_path =
            _http_session->server_config->root_path + _http_session->request->target.directory_path;

    if (_http_session->server_config->cgi_file_extensions.find(_http_session->request->target.extension) !=
        _http_session->server_config->cgi_file_extensions.end()) {
        is_cgi = true;
    }
    else {
        if (file_path == directory_path) {
            file_path += _http_session->server_config->default_file_name;
        }
        is_cgi = false;
    }
}

template<class CoreModule>
void HttpProcessRequestEvent<CoreModule>::ProcessMethod() {
    _http_session->request->method = Http::GetMethod(_http_session->request->raw_method);
    switch (_http_session->request->method) {
        case Http::Method::GET:
        case Http::Method::DELETE:
        case Http::Method::HEAD:
            break;

        case Http::Method::POST:
            if (!_http_session->request->content_length.HasValue()) {
                throw LengthRequired("Length required", _http_session->server_config);
            }
            break;

        case Http::Method::PUT:
        case Http::Method::OPTIONS:
        case Http::Method::CONNECT:
        case Http::Method::TRACE:
        case Http::Method::PATCH:
            throw NotImplemented("Method " + _http_session->request->raw_method + " is not implemented",
                                 _http_session->server_config);

        case Http::Method::UNKNOWN:
            throw MethodNotAllowed("Method not allowed", _http_session->server_config);
    }
}

template<class CoreModule>
void HttpProcessRequestEvent<CoreModule>::RunCgiPipeline(const std::string& file_path) {

    _http_session->state = HttpSessionState::ProcessResource;
}

template<class CoreModule>
void HttpProcessRequestEvent<CoreModule>::RunFilePipeline(const std::string& file) {
    int fd = open(file.c_str(), O_RDONLY);
    if (fd == -1) {
        throw NotFound("File not found or not available", _http_session->server_config);
    }
    if (!SetSocketNonBlocking(fd)) {
        close(fd);
        throw std::runtime_error("Failed to set socket non blocking");
    }

    SharedPtr<Session<CoreModule> > file_session = MakeShared<Session<CoreModule> >(
            new HttpFileSession<CoreModule>(_packaged_http_session->core_module->GetNextSessionIndex(),
                                            _packaged_http_session->core_module, SocketFd(fd),
                                            _packaged_http_session));

    _http_session->core_module->AddSession(fd, file_session);  /// core_module will invoke read events for this file fd
    _http_session->state = HttpSessionState::ProcessResource;
}

template<class CoreModule>
void HttpProcessRequestEvent<CoreModule>::Process() {
    if (!_packaged_http_session->available) {
        LOG_INFO(GetName(), " on closed connection");
        return;
    }

    _http_session = dynamic_cast<HttpSession<CoreModule>*>(_packaged_http_session.Get());
    if (_http_session == nullptr) {
        LOG_ERROR(GetName(), " on non-http session");
        return;
    }

    if (_http_session->state != HttpSessionState::ProcessRequest) {
        LOG_INFO(GetName(), " on wrong connection state");
        return;
    }

    try {
        ProcessHttpVersion();

        std::string file_path;
        bool is_cgi = false;
        ProcessFilePath(file_path, is_cgi);

        ProcessHostHeader();
        ProcessConnectionHeader();
        ProcessKeepAliveHeader();
        ProcessMethod();

        if (is_cgi) {
            RunCgiPipeline(file_path);
        }
        else {
            RunFilePipeline(file_path);
        }
    }
    catch (const HttpException& e) {
        _http_session->SendDataToClient(e.GetErrorResponse()->response, e.ShouldKeepAlive());
        LOG_INFO("HttpException: ", e.what());
    }
    catch (const std::exception& e) {
        LOG_ERROR("Unexpected exception: ", e.what());
        _http_session->SendDataToClient(
                Response::MakeErrorResponse(Http::InternalServerError,
                                            "Internal server error",
                                            _http_session->server_config).response,
                false);
    }

}
