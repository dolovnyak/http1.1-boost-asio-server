#include "Poll.h"
#include "logging.h"

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

std::vector<ServerInstance> Poll::Setup(const Config& config) {
    _poll_fds_number = 0;
    int option_value = 1;

    _poll_fds = (pollfd*)calloc(config.GetMaxEventsNumber(), sizeof(struct pollfd));
    if (_poll_fds == nullptr) {
        LOG_ERROR("Failed to allocate memory for poll_fds");
        return {};
    }

    for (const auto& server_config: config.GetServersConfigs()) {

        int listening_socket_fd = socket(AF_INET, SOCK_STREAM, 0); /// TODO maybe also support AF_INET6 from config?
        if (listening_socket_fd == -1) {
            LOG_ERROR("Failed to create listening socket");
            return {};
        }

        if (setsockopt(listening_socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&option_value, sizeof(option_value)) < 0) {
            Log::Perror("Failed to set socket options");
            close(listening_socket_fd);
            return {};
        }

        if (ioctl(listening_socket_fd, FIONBIO, (char*)&option_value) < 0) {
            LOG_PERROR("Failed to set socket non blocking");
            close(listening_socket_fd);
            return {};
        }

        struct sockaddr_in address{};
        bzero(&address, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = htons(server_config.port);

        if (bind(listening_socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            LOG_PERROR("Failed to bind socket");
            close(listening_socket_fd);
            return {};
        }

        if (listen(listening_socket_fd, server_config.max_connection_number) < 0) {
            LOG_PERROR("Failed to set listen backlog");
            close(listening_socket_fd);
            return {};
        }

        _poll_fds[_poll_fds_number].fd = listening_socket_fd;
        _poll_fds[_poll_fds_number].events = POLLIN;
        ++_poll_fds_number;

        _server_instances.emplace_back(listening_socket_fd);
    }

    if (_server_instances.empty()) {
        LOG_ERROR("No server instances created");
    }
    return _server_instances;
}

void Poll::ProcessEvents(int timeout) {

    LOG_DEBUG("Before Poll, available sockets num: ", _poll_fds_number);
    if (poll(_poll_fds, _poll_fds_number, timeout) < 0) {
        LOG_PERROR("Failed to poll");
        return;
    }
    LOG_DEBUG("After Poll");

    int current_size = _poll_fds_number;
    for (int i = 0; i < current_size; i++) {
        LOG_DEBUG("Poll fd: ", _poll_fds[i].fd, "; Poll event_system: ", _poll_fds[i].events, "; Poll revents ",
                  _poll_fds[i].revents);

        if (_poll_fds[i].revents == 0)
            continue;
        if (_poll_fds[i].fd == -1) {
            LOG_WARNING("Poll fd is -1");
        }

        /// TODO remake it on hash map or something like this, for now it's shit
        bool is_listening_socket = false;
        for (auto& server_instance: _server_instances) {
            if (server_instance.listening_socket_fd == _poll_fds[i].fd) {
                is_listening_socket = true;
                ProcessNewConnection(i);
            }
            break;
        }

        if (is_listening_socket) {
            break;
        }

        if (_poll_fds[i].revents & POLLIN) {
            ProcessRead(i);
        }
        else if (_poll_fds[i].revents & POLLOUT) {
            ProcessWrite(i);
        }
        else {
            if (_poll_fds[i].revents & (POLLHUP | POLLNVAL)) {
                LOG_INFO("Connection closed: ", _poll_fds[i].fd);
                CloseSocket(i);
            }
            else {
                LOG_ERROR("Incorrect revents value: ", _poll_fds[i].revents);
                return;
            }
        }

    }

    ProcessCompress();
}

void Poll::ProcessRead(int index) {
    LOG_INFO("Read processing");
    char buffer[READ_BUFFER_SIZE];

    for (;;) {
        LOG_DEBUG("Reading...");
        ssize_t bytes_read = recv(_poll_fds[index].fd, buffer, sizeof(buffer), 0);
        LOG_DEBUG("Bytes read: ", bytes_read);

        if (bytes_read < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                /// TODO parse event here
                LOG_INFO("Finish reading");
                _poll_fds[index].events = POLLOUT;
                break;
            }
            else {
                LOG_PERROR("Failed to read from socket");
                CloseSocket(index);
                break;
            }
        }
        else if (bytes_read == 0) {
            LOG_INFO("Connection closed by client: ", _poll_fds[index].fd);
            CloseSocket(index);
            break;
        }
    }
}

void Poll::ProcessWrite(int index) {
    LOG_INFO("Write processing");
    const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!  ";
    ssize_t bytes_send = send(_poll_fds[index].fd, hello, strlen(hello), 0);

    if (bytes_send < 0) {
        LOG_PERROR("Failed to send data");
    }

    CloseSocket(index);
    LOG_INFO("Bytes send: ", bytes_send);
}

void Poll::ProcessNewConnection(int index) {
    LOG_INFO("New connections processing");
    for (;;) {
        int new_connection = accept(_poll_fds[index].fd, nullptr, nullptr); // TODO maybe fill address from this
        if (new_connection == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                LOG_INFO("Finish processing new connections");
                break;
            }
            else {
                LOG_PERROR("Failed to accept new connection");
                break;
            }
        }
        LOG_INFO("New connection: ", new_connection);
        _poll_fds[_poll_fds_number].fd = new_connection;
        _poll_fds[_poll_fds_number].events = POLLIN;
        ++_poll_fds_number;
    }
}

void Poll::CloseSocket(int index) {
    close(_poll_fds[index].fd);
    _poll_fds[index].fd = -1;
    _should_compress = true;
}

void Poll::ProcessCompress() {
    if (_should_compress) {
        LOG_INFO("Compress fds processing");
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
}
