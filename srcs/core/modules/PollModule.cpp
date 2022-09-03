#include "PollModule.h"
#include "Logging.h"
#include "HttpSession.h"
#include "EventMaker.h"

#include <sys/poll.h>
#include <sys/socket.h>

PollModule::PollModule(const SharedPtr<Config>& config, std::queue<SharedPtr<Event> >* event_queue)
        : _config(config),
          _event_queue(event_queue),
          _poll_current_index(0),
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

        _poll_fds[_poll_current_index].fd = socket;
        _poll_fds[_poll_current_index].events = POLLIN;
        ++_poll_current_index;

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

    LOG_DEBUG("Before PollModule, available sockets num: ", _poll_current_index);
    if (poll(_poll_fds, _poll_current_index, timeout) < 0) {
        LOG_PERROR("Failed to poll");
        return;
    }

    int current_size = _poll_current_index;
    for (int i = 0; i < current_size; i++) {
        LOG_DEBUG("PollModule fd: ", _poll_fds[i].fd, "; PollModule event_system: ", _poll_fds[i].events,
                  "; PollModule revents ", _poll_fds[i].revents);

        if (_poll_fds[i].revents == 0)
            continue;
        if (_poll_fds[i].fd == -1) {
            LOG_WARNING("PollModule fd is -1");
        }

        if (_servers.find(_poll_fds[i].fd) != _servers.end()) {
            ProcessInnerNewHttpSessions(i);
        }
        else if (_poll_fds[i].revents & POLLIN) {
            ProcessInnerRead(i);
        }
        else if (_poll_fds[i].revents & POLLOUT) {
            ProcessInnerWrite(i);
        }
        else {
            if (_poll_fds[i].revents & (POLLHUP | POLLNVAL)) {
                LOG_INFO("HttpSession closed: ", _poll_fds[i].fd);
                CloseSession(i);
            }
            else {
                LOG_ERROR("Incorrect revents value: ", _poll_fds[i].revents);
                return;
            }
        }
    }
}

void PollModule::ProcessInnerNewHttpSessions(int poll_index) {
    LOG_INFO("New sessions processing");

    if (_poll_current_index >= _config->max_sockets_number) {
        LOG_ERROR("Max sockets number reached");
        return;
    }

    for (;;) {
        int socket = accept(_poll_fds[poll_index].fd, nullptr, nullptr); // TODO maybe fill address from this
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
                new HttpSession<PollModule>(GetCoreModuleIndex(), this, socket,
                                            _servers.at(_poll_fds[poll_index].fd)));
        AddSession(socket, session);
    }
}

void PollModule::AddSession(int socket, const SharedPtr<Session<PollModule> >& session) {
    LOG_INFO("New session: ", socket);

    _sessions.insert(std::pair<int, SharedPtr<Session<PollModule> > >(_poll_current_index, session));
    _poll_fds[_poll_current_index].fd = socket;
    _poll_fds[_poll_current_index].events = POLLIN;
    ++_poll_current_index;
}

void PollModule::CloseSession(int poll_index) {
    LOG_INFO("Close connection: ", _poll_fds[poll_index].fd);

    _sessions.at(poll_index)->available = false;
    close(_poll_fds[poll_index].fd);
    _sessions.erase(poll_index);
    _poll_fds[poll_index].fd = -1;
    _should_compress = true;
}


void PollModule::ProcessInnerRead(int poll_index) {
    LOG_INFO("Read processing");

    LOG_DEBUG("Reading...");
    ssize_t bytes_read = read(_poll_fds[poll_index].fd, _read_buffer, _read_buffer_size);

    if (bytes_read < 0) {
        LOG_PERROR("Failed to read from socket");
        CloseSession(poll_index);
    }
    else if (bytes_read == 0) {
        SharedPtr<Event> read_zero_bytes_event = EventMaker<PollModule>::MakeReadZeroBytesEvent(
                _sessions.at(poll_index),
                _event_queue);
        _event_queue->push(read_zero_bytes_event);
    }
    else {
        SharedPtr<std::string> raw_request_part = MakeShared<std::string>(std::string(_read_buffer, bytes_read));
        LOG_DEBUG("Bytes read: ", bytes_read);
        SharedPtr<Event> read_event = EventMaker<PollModule>::MakeReadEvent(_sessions.at(poll_index),
                                                                            raw_request_part,
                                                                            _event_queue);
        _event_queue->push(read_event);
    }
}

void PollModule::ProcessInnerWrite(int poll_index) {
    LOG_INFO("Write processing");

    SharedPtr<Session<PollModule> > session = _sessions.at(poll_index);
    std::string response = session->GetResponseData();

    /// TODO maybe it will be needed to make chunked write.
    ssize_t bytes_send = send(_poll_fds[poll_index].fd, response.c_str(), response.size(), 0);

    if (bytes_send < 0) {
        LOG_PERROR("Failed to send data");
    }
    else if (bytes_send == 0) {
        LOG_WARNING("Zero bytes send");
    }
    else if (static_cast<size_t>(bytes_send) < response.size()) {
        LOG_INFO("Bytes send: ", bytes_send);
        return;
    }
    else {
        LOG_INFO("Bytes send: ", bytes_send);
        _poll_fds[poll_index].events = POLLIN;

        SharedPtr<Event> after_write_event = EventMaker<PollModule>::MakeAfterWriteEvent(session, _event_queue);
        _event_queue->push(after_write_event);
    }
}

void PollModule::ProcessCompress() {
    if (!_should_compress)
        return;
    _should_compress = false;

    LOG_INFO("Compressing...");
    for (int i = 0; i < _poll_current_index; ++i) {
        if (_poll_fds[i].fd == -1) {
            for (int j = i; j < _poll_current_index; ++j) {
                _poll_fds[j].fd = _poll_fds[j + 1].fd;
            }
            --i;
            --_poll_current_index;
        }
    }
}

void PollModule::SendDataToSocket(int poll_index) {
    _poll_fds[poll_index].events = POLLOUT;
}

int PollModule::GetCoreModuleIndex() const {
    return _poll_current_index;
}