#pragma once

#include "Event.h"
#include "HttpException.h"
#include "utilities.h"

#include <queue>
#include <unistd.h>

template<class CoreModule>
class ProcessRequestEvent : public Event {
public:
    ProcessRequestEvent(const SharedPtr<HttpSession<CoreModule> >& connection,
                        std::queue<SharedPtr<Event> >* event_queue)
            : _session(connection), _event_queue(event_queue) {}

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

    SharedPtr<HttpSession<CoreModule> > _session;
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
        /// может открыть файл и отправить его
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
    if (!_session->request->content_length.HasValue()) {
        throw LengthRequired("Length required", _session->server_instance_info);
    }

    if (cgi) {
        /// запускает cgi процесс (передавая ему тело)
    }
    else {
        throw MethodNotAllowed("Method not allowed", _session->server_instance_info);
    }
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::ProcessDelete(const std::string& file, bool cgi) {
    if (cgi) {
        /// запускает cgi процесс
    }
    else {
        throw MethodNotAllowed("Method not allowed", _session->server_instance_info);
    }
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::ProcessHostHeader() {
    HeaderIterator it = _session->request->headers.find(HOST);
    if (it == _session->request->headers.end() || it->second.size() != 1) {
        throw BadRequest("Host header incorrect", _session->server_instance_info);
    }
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::ProcessConnectionHeader() {
    HeaderIterator it = _session->request->headers.find(CONNECTION);

    if (it != _session->request->headers.end()) {
        if (ToLower(it->second.back()) == KEEP_ALIVE) {
            _session->keep_alive = true;
        }
        else if (ToLower(it->second.back()) == CLOSE) {
            _session->keep_alive = false;
        }
    }
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::ProcessKeepAliveHeader() {
    HeaderIterator it = _session->request->headers.find(KEEP_ALIVE);
    const std::string& value = it->second.back();

    std::vector<std::string> tokens = SplitString(value, ",");
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::string parameter = StripString(tokens[i]);
        std::vector<std::string> parameter_tokens = SplitString(parameter, "=");

        if (parameter_tokens.size() == 2) {
            if (ToLower(parameter_tokens[0]) == TIMEOUT && IsPositiveNumberString(parameter_tokens[1])) {
                _session->keep_alive_timeout =
                        std::min(_session->server_instance_info->max_keep_alive_timeout,
                                 std::max(ParsePositiveInt(parameter_tokens[1]),
                                          _session->server_instance_info->default_keep_alive_timeout));
            }
        }
        /// MAX parameter is outdated and not supported
    }
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::Process() {
    try {
        if (!_session->available) {
            LOG_INFO(GetName(), " on closed connection");
            return;
        }
        if (_session->state != ConnectionState::ProcessRequest) {
            LOG_INFO(GetName(), " on wrong connection state");
            return;
        }

        ProcessHostHeader();
        ProcessConnectionHeader();
        ProcessKeepAliveHeader();

        std::string file_path = _session->server_instance_info->root_path + _session->request->target.full_path;
        std::string dir_path = _session->server_instance_info->root_path + _session->request->target.directory_path;
        bool cgi = false;

        if (_session->server_instance_info->cgi_directory_paths.find(dir_path) !=
            _session->server_instance_info->cgi_directory_paths.end()) {
            if (access(_session->request->target.full_path.c_str(), X_OK) == -1) {
                throw NotFound("File not found or not available", _session->server_instance_info);
            }
            cgi = true;
        }
        else {
            if (file_path == dir_path) {
                file_path += _session->server_instance_info->default_file_name;
            }
            if (access(file_path.c_str(), R_OK) == -1) {
                throw NotFound("File not found or not available", _session->server_instance_info);
            }
        }

        if (_session->request->method == "GET") {
            ProcessGet(file_path, cgi);
        }
        else if (_session->request->method == "HEAD") {
            ProcessHead(file_path, cgi);
        }
        else if (_session->request->method == "POST") {
            ProcessPost(file_path, cgi);
        }
        else if (_session->request->method == "DELETE") {
            ProcessDelete(file_path, cgi);
        }
        else if (_session->request->method == "PUT"
                 || _session->request->method == "OPTIONS"
                 || _session->request->method == "CONNECT"
                 || _session->request->method == "TRACE"
                 || _session->request->method == "PATCH") {
            throw NotImplemented("Method " + _session->request->method + " is not implemented",
                                 _session->server_instance_info);
        }
        else {
            throw MethodNotAllowed("Method not allowed", _session->server_instance_info);
        }
    }
    catch (const HttpException& e) {
        _session->SendDataToClient(e.GetErrorResponse(), e.ShouldKeepAlive());
        LOG_INFO("HttpException: ", e.what());
    }
    catch (const std::exception& e) {
        LOG_ERROR("Unexpected exception: ", e.what());
        _session->SendDataToClient(
                MakeShared(Response::MakeErrorResponse(Http::InternalServerError,
                                                       "Internal server error",
                                                       _session->server_instance_info)),
                false);
    }

}
