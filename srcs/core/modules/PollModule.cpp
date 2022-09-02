#include "PollModule.h"
#include "Logging.h"
#include "HttpSession.h"
#include "EventMaker.h"

#include <sys/poll.h>
#include <sys/socket.h>

PollModule::PollModule(const SharedPtr<Config>& config, std::queue<SharedPtr<Event> >* event_queue)
        : _config(config),
          _event_queue(event_queue),
          _session_last_id(0),
          _poll_fds(nullptr),
          _should_compress(false),
          _read_buffer_size(_config->read_buffer_size),
          _read_buffer(new char[_read_buffer_size]) {


    _poll_fds = (pollfd*)calloc(_config->max_sockets_number, sizeof(struct pollfd)); /// TODO remake on new
    if (_poll_fds == nullptr) {
        throw std::runtime_error("Failed to allocate memory for poll fds");
    }

    for (size_t i = 0; i < _config->servers_configs.size(); ++i) {
        int socket = Http::SetupSocket(_config->servers_configs[i], _config);

        _poll_fds[_session_last_id].fd = socket;
        _poll_fds[_session_last_id].events = POLLIN;
        ++_session_last_id;

        _servers.insert(std::pair<int, SharedPtr<ServerConfig> >(socket, _config->servers_configs[i]));
    }

    if (_servers.empty()) {
        throw std::runtime_error("No server instances created");
    }
}

PollModule::~PollModule() {
    delete[] _read_buffer;
    free(_poll_fds);
}

void PollModule::ProcessEvents(int timeout) {
    ProcessCompress();

    LOG_DEBUG("Before PollModule, available sockets num: ", _session_last_id);
    if (poll(_poll_fds, _session_last_id, timeout) < 0) {
        LOG_PERROR("Failed to poll");
        return;
    }

    int current_size = _session_last_id;
    for (int i = 0; i < current_size; i++) {
        LOG_DEBUG("PollModule fd: ", _poll_fds[i].fd, "; PollModule event_system: ", _poll_fds[i].events,
                  "; PollModule revents ", _poll_fds[i].revents);

        if (_poll_fds[i].revents == 0)
            continue;
        if (_poll_fds[i].fd == -1) {
            LOG_WARNING("PollModule fd is -1");
        }

        if (_servers.find(_poll_fds[i].fd) != _servers.end()) {
            ProcessNewHttpSessions(i);
        }
        else if (_poll_fds[i].revents & POLLIN) {
            ProcessRead(i);
        }
        else if (_poll_fds[i].revents & POLLOUT) {
            ProcessWrite(i);
        }
        else {
            if (_poll_fds[i].revents & (POLLHUP | POLLNVAL)) {
                LOG_INFO("HttpSession closed: ", _poll_fds[i].fd);
                CloseSocket(i);
            }
            else {
                LOG_ERROR("Incorrect revents value: ", _poll_fds[i].revents);
                return;
            }
        }
    }
}

void PollModule::ProcessNewHttpSessions(int index) {
    LOG_INFO("New sessions processing");
    for (;;) {
        int socket = accept(_poll_fds[index].fd, nullptr, nullptr); // TODO maybe fill address from this
        if (socket == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                LOG_INFO("Finish processing new sessions");
                break;
            }
            else {
                LOG_PERROR("Failed to accept new session");
                break;
            }
        }

        SharedPtr<Session<PollModule> > session = MakeShared<Session<PollModule> >(
                new HttpSession<PollModule>(_session_last_id, _servers.at(_poll_fds[index].fd), this));
        AddSession(socket, session);
    }
}

void PollModule::AddSession(int socket, const SharedPtr<Session<PollModule> >& session) {
    LOG_INFO("New session: ", socket);

    _sessions.insert(std::pair<int, SharedPtr<Session<PollModule> > >(_session_last_id, session));
    _poll_fds[_session_last_id].fd = socket;
    _poll_fds[_session_last_id].events = POLLIN;
    ++_session_last_id;
}

void PollModule::ProcessRead(int index) {
    LOG_INFO("Read processing");

    LOG_DEBUG("Reading...");
    ssize_t bytes_read = recv(_poll_fds[index].fd, _read_buffer, _read_buffer_size, 0);
    SharedPtr<std::string> raw_request_part = MakeShared<std::string>(std::string(_read_buffer, bytes_read));
    LOG_DEBUG("Bytes read: ", bytes_read);

    if (bytes_read < 0) {
        LOG_PERROR("Failed to read from socket");
        CloseSocket(index);
    }
    else if (bytes_read == 0) {
        LOG_INFO("HttpSession closed by client: ", _poll_fds[index].fd);
        CloseSocket(index);
    }
    else {
        SharedPtr<Event> read_event = EventMaker<PollModule>::MakeReadEvent(_sessions.at(index),
                                                                            raw_request_part,
                                                                            _event_queue);
        _event_queue->push(read_event);
    }
}

void PollModule::ProcessWrite(int index) {
    LOG_INFO("Write processing");

    SharedPtr<Session<PollModule> > session = _sessions.at(index);
    std::string response = session->response_data;

    /// TODO maybe it will be needed to make chunked write.
    ssize_t bytes_send = send(_poll_fds[index].fd, response.c_str(), response.size(), 0);

    if (bytes_send < 0) {
        LOG_PERROR("Failed to send data");
    }
    LOG_INFO("Bytes send: ", bytes_send);

    if (session->ShouldCloseAfterResponse()) {
        CloseSocket(index);
    }
}

void PollModule::CloseSocket(int socket_id) {
    LOG_INFO("Close connection: ", _poll_fds[socket_id].fd);

    _sessions.at(socket_id)->available = false;
    close(_poll_fds[socket_id].fd);
    _sessions.erase(socket_id);
    _poll_fds[socket_id].fd = -1;
    _should_compress = true;
}

void PollModule::ProcessCompress() {
    if (!_should_compress)
        return;
    _should_compress = false;

    LOG_INFO("Compressing...");
    for (int i = 0; i < _session_last_id; ++i) {
        if (_poll_fds[i].fd == -1) {
            for (int j = i; j < _session_last_id; ++j) {
                _poll_fds[j].fd = _poll_fds[j + 1].fd;
            }
            --i;
            --_session_last_id;
        }
    }
}

void PollModule::SendDataToSocket(int socket_id) {
    _poll_fds[socket_id].events = POLLOUT;
}