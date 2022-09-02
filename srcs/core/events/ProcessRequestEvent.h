#pragma once

#include "Event.h"
#include "HttpException.h"
#include "utilities.h"
#include "FileSession.h"
#include "FileHandler.h"

#include <queue>
#include <unistd.h>
#include <fcntl.h>

template<class CoreModule>
class ProcessRequestEvent : public Event {
public:
    ProcessRequestEvent(const SharedPtr<Session<CoreModule> >& session,
                        std::queue<SharedPtr<Event> >* event_queue)
            : _session(session), _event_queue(event_queue) {}

    ~ProcessRequestEvent() {}

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;


private:
    void ProcessGet(const std::string& file, bool cgi);

    void ProcessHead(const std::string& file, bool cgi);

    void ProcessPost(const std::string& file, bool cgi);

    void ProcessDelete(const std::string& file, bool cgi);


    void ProcessHostHeader();

    void ProcessConnectionHeader();

    void ProcessKeepAliveHeader();

    void ProcessFilePath(std::string& file, bool& cgi);

    SharedPtr<Session<CoreModule> > _session;

    HttpSession<CoreModule>* _http_session;

    std::queue<SharedPtr<Event> >* _event_queue;
};


template<class CoreModule>
const std::string& ProcessRequestEvent<CoreModule>::GetName() const {
    static std::string kName = "ProcessRequestEvent";
    return kName;
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::ProcessGet(const std::string& file, bool cgi) {
    if (cgi) {
        /// может запустить cgi процесс (не передавая ему тело)
    }
    else {

        FileHandler::Handle(_session, file, _event_queue);
        /// мне надо создать file_session и добавить его в core_module.
        /// в FileSession я положу HttpSession. и Для FileSession я добавлю рид ивент, который при прочтении всего файла, выдаст ответ.
//        SharedPtr<Session<CoreModule> > file_session(new FileSession<CoreModule>(_session, fd));
//        _http_session->core_module->AddSession(fd, file_session);
    }
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::ProcessHead(const std::string& file, bool cgi) {
    if (cgi) {
        /// запустить cgi процесс (не передавая ему тело)
    }
    else {
        ///
    }
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::ProcessPost(const std::string& file, bool cgi) {
    if (!_http_session->request->content_length.HasValue()) {
        throw LengthRequired("Length required", _http_session->server_config);
    }

    if (cgi) {
        /// запускает cgi процесс (передавая ему тело)
    }
    else {
        throw MethodNotAllowed("Method not allowed", _http_session->server_config);
    }
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::ProcessDelete(const std::string& file, bool cgi) {
    if (cgi) {
        /// запускает cgi процесс
    }
    else {
        throw MethodNotAllowed("Method not allowed", _http_session->server_config);
    }
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::ProcessHostHeader() {
    HeaderIterator it = _http_session->request->headers.find(HOST);
    if (it == _http_session->request->headers.end() || it->second.size() != 1) {
        throw BadRequest("Host header incorrect", _http_session->server_config);
    }
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::ProcessConnectionHeader() {
    HeaderIterator it = _http_session->request->headers.find(CONNECTION);

    if (it != _http_session->request->headers.end()) {
        if (ToLower(it->second.back()) == KEEP_ALIVE) {
            _http_session->keep_alive = true;
        }
        else if (ToLower(it->second.back()) == CLOSE) {
            _http_session->keep_alive = false;
        }
    }
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::ProcessKeepAliveHeader() {
    HeaderIterator it = _http_session->request->headers.find(KEEP_ALIVE);
    const std::string& value = it->second.back();

    std::vector<std::string> tokens = SplitString(value, ",");
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::string parameter = StripString(tokens[i]);
        std::vector<std::string> parameter_tokens = SplitString(parameter, "=");

        if (parameter_tokens.size() == 2) {
            if (ToLower(parameter_tokens[0]) == TIMEOUT && IsPositiveNumberString(parameter_tokens[1])) {
                _http_session->keep_alive_timeout =
                        std::min(_http_session->server_config->max_keep_alive_timeout,
                                 std::max(ParsePositiveInt(parameter_tokens[1]),
                                          _http_session->server_config->default_keep_alive_timeout));
            }
        }
        /// MAX parameter is outdated and not supported
    }
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::ProcessFilePath(std::string& file_path, bool& is_cgi) {
    file_path = _http_session->server_config->root_path + _http_session->request->target.full_path;
    std::string directory_path =
            _http_session->server_config->root_path + _http_session->request->target.directory_path;

    if (_http_session->server_config->cgi_file_extensions.find(_http_session->request->target.extension) !=
        _http_session->server_config->cgi_file_extensions.end()) {

        if (access(file_path.c_str(), X_OK) == -1) {
            throw NotFound("File not found or not available", _http_session->server_config);
        }
        is_cgi = true;
    }
    else {
        if (file_path == directory_path) {
            file_path += _http_session->server_config->default_file_name;
        }
        if (access(file_path.c_str(), R_OK) == -1) {
            throw NotFound("File not found or not available", _http_session->server_config);
        }
        is_cgi = false;
    }
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::Process() {
    if (!_session->available) {
        LOG_INFO(GetName(), " on closed connection");
        return;
    }

    _http_session = dynamic_cast<HttpSession<CoreModule>*>(_session.Get());
    if (_http_session == nullptr) {
        LOG_ERROR(GetName(), " on non-http session");
        return;
    }

    try {
        if (!_http_session->available) {
            LOG_INFO(GetName(), " on closed connection");
            return;
        }
        if (_http_session->state != ConnectionState::ProcessRequest) {
            LOG_INFO(GetName(), " on wrong connection state");
            return;
        }

        std::string file_path;
        bool is_cgi = false;
        ProcessFilePath(file_path, is_cgi);

        ProcessHostHeader();
        ProcessConnectionHeader();
        ProcessKeepAliveHeader();

        Http::Method method = Http::GetMethod(_http_session->request->method);
        switch (method) {
            case Http::Method::GET:
                ProcessGet(file_path, is_cgi);
                break;
            case Http::Method::HEAD:
                ProcessHead(file_path, is_cgi);
                break;
            case Http::Method::POST:
                ProcessPost(file_path, is_cgi);
                break;
            case Http::Method::DELETE:
                ProcessDelete(file_path, is_cgi);
                break;

            case Http::Method::PUT:
            case Http::Method::OPTIONS:
            case Http::Method::CONNECT:
            case Http::Method::TRACE:
            case Http::Method::PATCH:
                throw NotImplemented("Method " + _http_session->request->method + " is not implemented",
                                     _http_session->server_config);

            case Http::Method::UNKNOWN:
                throw MethodNotAllowed("Method not allowed", _http_session->server_config);
        }
    }
    catch (const HttpException& e) {
        _http_session->SendDataToClient(e.GetErrorResponse(), e.ShouldKeepAlive());
        LOG_INFO("HttpException: ", e.what());
    }
    catch (const std::exception& e) {
        LOG_ERROR("Unexpected exception: ", e.what());
        _http_session->SendDataToClient(
                MakeShared(Response::MakeErrorResponse(Http::InternalServerError,
                                                       "Internal server error",
                                                       _http_session->server_config)),
                false);
    }

}
