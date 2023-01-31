#pragma once

#include "Event.h"
#include "HttpException.h"
#include "utilities.h"
#include "HttpFileSession.h"

#include <queue>
#include <unistd.h>
#include <fcntl.h>

template<class CoreModule>
class HttpSessionProcessRequestEvent : public Event {
public:
    HttpSessionProcessRequestEvent(const std::shared_ptr<Session >& session,
                                   std::queue<std::shared_ptr<Event> >* event_queue)
            : _packaged_http_session(session), _event_queue(event_queue) {}

    ~HttpSessionProcessRequestEvent() {}

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;


private:
    void RunCgiPipeline();

    void RunFilePipeline();

    std::shared_ptr<Session > _packaged_http_session;

    HttpSession* _http_session;

    std::queue<std::shared_ptr<Event> >* _event_queue;
};


template<class CoreModule>
const std::string& HttpSessionProcessRequestEvent<CoreModule>::GetName() const {
    static std::string kName = "HttpSessionProcessRequestEvent";
    return kName;
}

template<class CoreModule>
void HttpSessionProcessRequestEvent<CoreModule>::RunCgiPipeline() {
    int fds[2];
    int err = pipe(fds);

    if (err == - 1) {
        throw InternalServerError("Pipe error", _http_session->port_servers_config);
    }

    ssize_t write_res = write(fds[1], _http_session->request->body.c_str(), _http_session->request->body.size());
    if (write_res == - 1) {
        throw InternalServerError("Error write to pipe", _http_session->port_servers_config);
    }

    int fork_id = fork();

    if (fork_id == - 1) {
        throw InternalServerError("Fork error", _http_session->port_servers_config);
    }
    else if (fork_id == 0) {
        char** env = new char*[_http_session->request->headers.size() + 1];

        err = dup2(fds[0], 0);
        if (err == - 1) {
            std::cout << "dup2 read error" << std::endl;
        }
        err = dup2(fds[1], 1);
        if (err == - 1) {
            std::cout << "dup2 write error" << std::endl;
        }

        env[0] = strdup("AUTH_TYPE=kabun");
        env[1] = NULL;

        char * const * nll = NULL;
        err = execve("/Users/sbecker/Desktop/projects/webserver-42/examples/cgi_checker/simple_cgi.py", nll, env);
        std::cout << "error: " << err << std::endl;
    }
    else {
        // parent process
        int status;
        waitpid(fork_id, &status, 0);
        std::cout << "status: " << status << std::endl;
        char buf[1024];
        ssize_t read_res = read(fds[0], buf, 1024);
        std::cout << "read_res: " << read_res << std::endl;
        std::cout << "buf: " << buf << std::endl;
    }
    /// нужно создать пайп. я получу дескриптор для записи и для чтения
    /// в дескриптор для записи нужно записать тело. и в мейн процессе забыть про него
    /// дескриптор для чтения нужно кинуть в пол и ждать из него ивенты на чтение

    /// нужно форкнуться и подменить стандартные потоки на дескрипторы пайпа
    /// нужно запустить cgi скрипт
    _http_session->state = HttpSessionState::ProcessResource;
}

template<class CoreModule>
void HttpSessionProcessRequestEvent<CoreModule>::RunFilePipeline() {
    int fd = open(_http_session->request->target.path.c_str(), O_RDONLY);
    if (fd == -1) {
        throw NotFound("File not found or not available", _http_session->port_servers_config);
    }
    if (!SetSocketNonBlocking(fd)) {
        close(fd);
        throw std::runtime_error("Failed to set socket non blocking");
    }

    std::shared_ptr<Session > file_session = MakeShared<Session >(
            new HttpFileSession(_packaged_http_session->core_module->GetNextSessionIndex(),
                                            _packaged_http_session->core_module, boost::asio::ip::tcp::socket(fd),
                                            _packaged_http_session));

    _http_session->core_module->AddSession(fd, file_session);  /// core_module will invoke read events for this file fd
    _http_session->state = HttpSessionState::ProcessResource;
}

template<class CoreModule>
void HttpSessionProcessRequestEvent<CoreModule>::Process() {
    if (!_packaged_http_session->available) {
        LOG_INFO(GetName(), " on closed connection");
        return;
    }

    _http_session = dynamic_cast<HttpSession*>(_packaged_http_session.Get());
    if (_http_session == nullptr) {
        LOG_ERROR(GetName(), " on non-http session");
        return;
    }

    if (_http_session->state != HttpSessionState::ProcessRequest) {
        LOG_INFO(GetName(), " on wrong connection state");
        return;
    }

    try {
        _http_session->request->Process();

        _http_session->keep_alive = _http_session->request->keep_alive;
        _http_session->keep_alive_timeout = _http_session->request->keep_alive_timeout;

        if (_http_session->request->is_cgi) {
            RunCgiPipeline();
        }
        else {
            RunFilePipeline();
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
                                            _http_session->port_servers_config).response,
                false);
    }

}
