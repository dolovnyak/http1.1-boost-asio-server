#include "PollModule.h"
#include "Logging.h"
#include "HttpSession.h"
#include "EventMaker.h"

#include <sys/poll.h>
#include <sys/socket.h>

PollModule::PollModule(const SharedPtr<Config>& config,
                       std::queue<SharedPtr<Event> >* event_queue,
                       std::unordered_map<SocketFd, SharedPtr<Session<PollModule> > >* sessions)
        : _config(config),
          _event_queue(event_queue),
          _sessions(sessions),
          _poll_index(0),
          _poll_fds_size(_config->max_sockets_number),
          _poll_fds(new struct pollfd[_poll_fds_size]),
          _should_compress(false),
          _read_buffer_size(_config->read_buffer_size),
          _read_buffer(new char[_read_buffer_size]) {}

PollModule::~PollModule() {
    delete[] _read_buffer;
    delete[] _poll_fds;
}

void PollModule::ProcessEvents(int timeout) {
    ProcessCompress();

    LOG_DEBUG("Before PollModule, available sockets num: ", _poll_index);
    if (poll(_poll_fds, _poll_index, timeout) < 0) {
        LOG_PERROR("Failed to poll");
        return;
    }

    int current_size = _poll_index;
    for (int index = 0; index < current_size; ++index) {
        struct pollfd& poll_fd = _poll_fds[index];

        LOG_DEBUG("PollModule fd: ", poll_fd.fd, "; ",
                  "PollModule event: ", poll_fd.events, "; ",
                  "PollModule revents ", poll_fd.revents);


        if (poll_fd.fd == -1) {
            LOG_WARNING("PollModule fd is -1 in poll events process");
            _should_compress = true;
            continue;
        }

        if (poll_fd.revents == 0) {
            continue;
        }

        Session<PollModule>::It session_iterator = _sessions->find(SocketFd(poll_fd.fd));
        if (session_iterator == _sessions->end()) {
            LOG_WARNING("There is no session by fd: ", poll_fd.fd);
            CloseSocket(index);
            continue;
        }

        if (poll_fd.revents & ~(POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL)) {
            LOG_WARNING("Unknown poll event: ", poll_fd.revents, " on fd: ", poll_fd.fd);
            CloseSession(index);
            continue;
        }

        if (poll_fd.revents & POLLNVAL) {
            LOG_INFO("PollModule: POLLNVAL on Client session: ", poll_fd.fd);
            CloseSession(index);
            continue;
        }
        if (poll_fd.revents & POLLHUP) {
            LOG_INFO("PollModule: POLLHUP on Client session: ", poll_fd.fd);
            CloseSession(index);
            continue;
        }

        if (poll_fd.revents & POLLERR) {
            LOG_ERROR("PollModule: POLLERR on fd: ", poll_fd.fd);
            CloseSession(index);
            continue;
        }

        if (poll_fd.revents & POLLIN) {
            ProcessInnerRead(index);
        }
        if (poll_fd.revents & POLLOUT) {
            ProcessInnerWrite(index);
        }

    }

}

void PollModule::ProcessInnerNewHttpSessions(int poll_index, SharedPtr<ServerConfig> server_config) {
    LOG_DEBUG("New sessions processing");

    if (_poll_index >= _config->max_sockets_number) {
        LOG_ERROR("Max sockets number reached");
        return;
    }

    for (;;) {
        int socket = accept(_poll_fds[poll_index].fd, nullptr, nullptr);
        if (socket == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                LOG_DEBUG("Finish processing new sessions");
                break;
            }
            else {
                LOG_PERROR("Failed to accept new session");
                break;
            }
        }

        SharedPtr<Session<PollModule> > session = MakeShared<Session<PollModule> >(
                new HttpSession<PollModule>(GetNextSessionIndex(), this, SocketFd(socket), server_config));
        AddSession(socket, session);
    }
}

void PollModule::AddSession(int socket, const SharedPtr<Session<PollModule> >& session) {
    LogSession(session, "Added session");

    _sessions->insert(std::pair<SocketFd, SharedPtr<Session<PollModule> > >(SocketFd(socket), session));
    _poll_fds[_poll_index].fd = socket;
    _poll_fds[_poll_index].events = POLLIN;
    ++_poll_index;
}

void PollModule::CloseSession(int poll_index) {
    SocketFd socket(_poll_fds[poll_index].fd);

    CloseSocket(poll_index);

    Session<PollModule>::It session_it = _sessions->find(socket);

    if (session_it == _sessions->end()) {
        LOG_WARNING("Close session: session not found");
        return;
    }
    LogSession(session_it->second, "Session closed");

    session_it->second->available = false;
    _sessions->erase(session_it);
}

void PollModule::CloseSocket(int poll_index) {
    close(_poll_fds[poll_index].fd);
    _poll_fds[poll_index].fd = -1;
    _should_compress = true;
}

void PollModule::ProcessInnerRead(int poll_index) {
    SharedPtr<Session<PollModule> > session = _sessions->at(SocketFd(_poll_fds[poll_index].fd));
    session->UpdateLastActivityTime();

    if (session->GetType() == SessionType::Server) {
        ServerSession<PollModule>* server_session = dynamic_cast<ServerSession<PollModule>*>(session.Get());
        ProcessInnerNewHttpSessions(poll_index, server_session->server_config);
        return;
    }

    LogSession(session, "Read processing");

    ssize_t bytes_read = read(_poll_fds[poll_index].fd, _read_buffer, _read_buffer_size);
    if (bytes_read < 0) {
        LOG_PERROR("Failed to read from socket");
        CloseSession(poll_index);
    }
    else if (bytes_read == 0) {
        SharedPtr<Event> read_zero_bytes_event = EventMaker<PollModule>::MakeReadZeroBytesEvent(session, _event_queue);
        _event_queue->push(read_zero_bytes_event);
    }
    else {
        SharedPtr<std::string> raw_request_part = MakeShared<std::string>(std::string(_read_buffer, bytes_read));
        LOG_DEBUG("Bytes read: ", bytes_read);
        SharedPtr<Event> read_event = EventMaker<PollModule>::MakeReadEvent(session,
                                                                            raw_request_part,
                                                                            _event_queue);
        _event_queue->push(read_event);
    }
}

void PollModule::ProcessInnerWrite(int poll_index) {
    Session<PollModule>::It session_it = _sessions->find(SocketFd(_poll_fds[poll_index].fd));
    if (session_it == _sessions->end()) {
        LOG_WARNING("Write processing: session not found");
        return;
    }

    Session<PollModule>::Ptr session = session_it->second;
    session->UpdateLastActivityTime();
    LogSession(session, "Write processing");

    if (session->GetType() == SessionType::Http) {
        HttpSession<PollModule>* http_session = dynamic_cast<HttpSession<PollModule>*>(session.Get());
        LOG_INFO("Http session write on state: ", HttpSessionState::ToString(http_session->state));
    }

    /// TODO maybe it will be needed to make chunked write.

    std::string response = session->GetResponseData();
    ssize_t bytes_send = send(_poll_fds[poll_index].fd, response.c_str(), response.size(), 0);

    if (bytes_send < 0) {
        LOG_PERROR("Failed to send data");
    }
    else if (bytes_send == 0) {
        LOG_WARNING("Zero bytes send, close session");
        CloseSession(poll_index);
    }
    else if (static_cast<size_t>(bytes_send) < response.size()) {
        LOG_INFO("Bytes send: ", bytes_send);
        return;
    }
    else {
        LogSession(session, "Bytes send " + std::to_string(bytes_send));
        _poll_fds[poll_index].events = POLLIN;

        SharedPtr<Event> after_write_event = EventMaker<PollModule>::MakeAfterWriteEvent(session, _event_queue);
        _event_queue->push(after_write_event);
    }
}

void PollModule::ProcessCompress() {
    if (!_should_compress)
        return;
    _should_compress = false;
    Session<PollModule>::It session_it;

    LOG_INFO("Compressing...");
    for (int i = 0; i < _poll_index;) {
        if (_poll_fds[i].fd == -1) {
            int i_end = i;
            while (_poll_fds[i_end].fd == -1 && i_end < _poll_index) {
                ++i_end;
            }
            int skip_size = i_end - i;

            int j_end = _poll_index - skip_size;
            for (int j = i; j < j_end; ++j) {

                _poll_fds[j] = _poll_fds[j + skip_size];

                session_it = _sessions->find(SocketFd(_poll_fds[j].fd));
                if (session_it != _sessions->end()) {
                    session_it->second->core_module_index = j;
                }
            }
            i = i_end;
            _poll_index -= skip_size;
        }
        else {
            ++i;
        }
    }
}

void PollModule::SendDataToSocket(int poll_index) {
    _poll_fds[poll_index].events = POLLOUT;
}

int PollModule::GetNextSessionIndex() const {
    if (_poll_index >= _poll_fds_size) {
        throw std::runtime_error("No free session index");
    }
    return _poll_index;
}