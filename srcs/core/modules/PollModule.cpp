#include "PollModule.h"
#include "Logging.h"
#include "HandleRequestEvent.h"

#include <sys/poll.h>
#include <sys/socket.h>

PollModule::PollModule(const SharedPtr<Config>& config, std::queue<SharedPtr<Event> >* event_queue)
        : _config(config),
          _event_queue(event_queue),
          _poll_fds_number(0),
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

        _poll_fds[_poll_fds_number].fd = socket;
        _poll_fds[_poll_fds_number].events = POLLIN;
        ++_poll_fds_number;

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

    LOG_DEBUG("Before PollModule, available sockets num: ", _poll_fds_number);
    if (poll(_poll_fds, _poll_fds_number, timeout) < 0) {
        LOG_PERROR("Failed to poll");
        return;
    }

    int current_size = _poll_fds_number;
    for (int i = 0; i < current_size; i++) {
        LOG_DEBUG("PollModule fd: ", _poll_fds[i].fd, "; PollModule event_system: ", _poll_fds[i].events,
                  "; PollModule revents ", _poll_fds[i].revents);

        if (_poll_fds[i].revents == 0)
            continue;
        if (_poll_fds[i].fd == -1) {
            LOG_WARNING("PollModule fd is -1");
        }

        if (_servers.find(_poll_fds[i].fd) != _servers.end()) {
            ProcessNewConnection(i);
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

void PollModule::ProcessNewConnection(int index) {
    LOG_INFO("New connections processing");
    for (;;) {
        int new_connection_fd = accept(_poll_fds[index].fd, nullptr, nullptr); // TODO maybe fill address from this
        if (new_connection_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                LOG_INFO("Finish processing new connections");
                break;
            }
            else {
                LOG_PERROR("Failed to accept new session");
                break;
            }
        }
        LOG_INFO("New session: ", new_connection_fd);
        _poll_fds[_poll_fds_number].fd = new_connection_fd;
        _poll_fds[_poll_fds_number].events = POLLIN;

        SharedPtr<HttpSession<PollModule> > session = MakeShared(
                HttpSession<PollModule>(_poll_fds_number, _servers.at(_poll_fds[index].fd), this));

        _sessions.insert(std::pair<int, SharedPtr<HttpSession<PollModule> > >(_poll_fds_number, session));

        ++_poll_fds_number;
    }
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
        _event_queue->push(MakeShared<Event>(new HandleRequestEvent<PollModule>(
                _sessions.at(index),
                raw_request_part,
                _event_queue)));
    }
}

void PollModule::ProcessWrite(int index) {
    LOG_INFO("Write processing");

    SharedPtr<HttpSession<PollModule> > connection = _sessions.at(index);
    std::string response = connection->response->raw_response;

    /// TODO maybe it will be needed to make chunked write.
    ssize_t bytes_send = send(_poll_fds[index].fd, response.c_str(), response.size(), 0);

    if (bytes_send < 0) {
        LOG_PERROR("Failed to send data");
    }
    LOG_INFO("Bytes send: ", bytes_send);

    if (!connection->keep_alive) {
        CloseSocket(index);
    }
}

void PollModule::CloseSocket(int index) {
    LOG_INFO("Close connection: ", _poll_fds[index].fd);
    _sessions.at(index)->available = false;
    close(_poll_fds[index].fd);
    _sessions.erase(index);
    _poll_fds[index].fd = -1;
    _should_compress = true;
}

void PollModule::ProcessCompress() {
    if (!_should_compress)
        return;
    _should_compress = false;

    LOG_INFO("Compressing...");
    for (int i = 0; i < _poll_fds_number; ++i) {
        if (_poll_fds[i].fd == -1) {
            for (int j = i; j < _poll_fds_number; ++j) {
                _poll_fds[j].fd = _poll_fds[j + 1].fd;
            }
            --i;
            --_poll_fds_number;
        }
    }
}

void PollModule::SendDataToClient(int index) {
    _poll_fds[index].events = POLLOUT;
}