#include "WebServer.h"

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

bool WebServer::Setup(const Config& config) {
    _config = config;
    _poll_fds_number = 0;
    int option_value = 1;

    /// TODO i don't know will it work if I use std allocator, need to check
    _poll_fds = (pollfd*)calloc(config.GetMaxEventsNumber(), sizeof(struct pollfd));
    if (_poll_fds == nullptr) {
        LOG_ERROR("Failed to allocate memory for poll_fds");
        return false;
    }

    for (const auto& server_config: config.GetServersConfigs()) {

        int listening_socket_fd = socket(AF_INET, SOCK_STREAM, 0); /// TODO maybe also support AF_INET6 from config?
        if (listening_socket_fd == -1) {
            LOG_ERROR("Failed to create listening socket");
            return false;
        }

        if (setsockopt(listening_socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&option_value, sizeof(option_value)) < 0) {
            Log::Perror("Failed to set socket options");
            close(listening_socket_fd);
            return false;
        }

        if (ioctl(listening_socket_fd, FIONBIO, (char*)&option_value) < 0) {
            LOG_PERROR("Failed to set socket non blocking");
            close(listening_socket_fd);
            return false;
        }

        struct sockaddr_in address{};
        bzero(&address, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = htons(server_config.port);

        if (bind(listening_socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            LOG_PERROR("Failed to bind socket");
            close(listening_socket_fd);
            return false;
        }

        if (listen(listening_socket_fd, server_config.max_connection_number) < 0) {
            LOG_PERROR("Failed to set listen backlog");
            close(listening_socket_fd);
            return false;
        }

        /// add listening socket to poll
        _poll_fds[_poll_fds_number].fd = listening_socket_fd;
        _poll_fds[_poll_fds_number].events = POLLIN;
        ++_poll_fds_number;

        ServerInstance server_instance(listening_socket_fd);
        _web_server_instances.push_back(server_instance);
    }
    return true;
}

void WebServer::PollProcessing(int timeout) {
    LOG_INFO("Before Poll ", _poll_fds_number);
    if (poll(_poll_fds, _poll_fds_number, timeout) < 0) {
        LOG_PERROR("Failed to poll");
        return; /// TODO maybe we should exit?
    }
    LOG_INFO("After Poll");
    int current_size = _poll_fds_number;
    bool compress_fds = false;

    for (int i = 0; i < current_size; i++) {
        LOG_INFO("Poll fd: ", _poll_fds[i].fd, "; Poll events: ", _poll_fds[i].events, "; Poll revents ", _poll_fds[i].revents);
        if (_poll_fds[i].revents == 0)
            continue;
        if (_poll_fds[i].fd == -1) {
            LOG_WARNING("Poll fd is -1");
        }

        int socket_fd = _poll_fds[i].fd;

        /// TODO remake it on hash map or something like this, for now it's shit
        bool is_listening_socket = false;
        for (auto& server_instance : _web_server_instances) {

            if (server_instance.listening_socket_fd == socket_fd) {
                LOG_INFO("New connections processing");
                is_listening_socket = true;
                for (;;) {
                    int new_connection = accept(socket_fd, nullptr, nullptr); // TODO fill address from this
                    if (new_connection == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            LOG_INFO("Finish processing new connections");
                            break;
                        }
                        else {
                            LOG_PERROR("Failed to accept new connection");
                            break; /// TODO maybe we should exit?
                        }
                    }
                    LOG_INFO("New connection: ", new_connection);
                    _poll_fds[_poll_fds_number].fd = new_connection;
                    _poll_fds[_poll_fds_number].events = POLLIN;
                    ++_poll_fds_number;
                }
                break;
            }

        }

        if (is_listening_socket) {
            break;
        }
        if (_poll_fds[i].revents & POLLIN) {
            LOG_INFO("Read socket");
            char buffer[1024];

            for (;;) {
                LOG_INFO("Reading...");
                ssize_t bytes_read = recv(_poll_fds[i].fd, buffer, sizeof(buffer), 0);
                LOG_INFO("Bytes read: ", bytes_read);

                if (bytes_read < 0) {
                    if (errno == EWOULDBLOCK || errno == EAGAIN) {
                        LOG_INFO("Finish reading");
                        _poll_fds[i].events = POLLOUT;
//                        LOG_INFO("Connection closed: ", _poll_fds[i].fd);
//                        close(_poll_fds[i].fd);
//                        _poll_fds[i].fd = -1;
//                        compress_fds = true;
                        break;
                    }
                    else {
                        LOG_PERROR("Failed to read from socket");
                        close(_poll_fds[i].fd);
                        _poll_fds[i].fd = -1;
                        compress_fds = true;
                        break;
                    }
                }
                else if (bytes_read == 0) {
                    LOG_INFO("Connection closed by client: ", _poll_fds[i].fd);
                    close(_poll_fds[i].fd);
                    _poll_fds[i].fd = -1;
                    compress_fds = true;
                    break;
                }
            }
        }
        else if (_poll_fds[i].revents & POLLOUT) {
            LOG_INFO("Write socket");
            const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!  ";
            ssize_t bytes_send = send(_poll_fds[i].fd, hello, strlen(hello), 0);

            if (bytes_send < 0) {
                LOG_PERROR("Failed to send data");
            }

            close(_poll_fds[i].fd);
            _poll_fds[i].fd = -1;
            compress_fds = true;

            LOG_INFO("Bytes send: ", bytes_send);
        }
        else {
            if (_poll_fds[i].revents & (POLLHUP | POLLNVAL)) {
                LOG_INFO("Connection closed: ", _poll_fds[i].fd);
                close(_poll_fds[i].fd);
                _poll_fds[i].fd = -1;
                compress_fds = true;
            }
            else {
                LOG_ERROR("Incorrect revents value: ", _poll_fds[i].revents);
                return; /// TODO maybe we should exit?
            }
        }

    }

    LOG_INFO("Before compress");
    if (compress_fds) {
        LOG_INFO("Compress fds");
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

void WebServer::Run() {
    while (true) {
        /// timeout is set to infinity when we don't have event in process and to 0 if we have
        PollProcessing(-1);
    }
}


/// for now make only poll support without thoughts about epoll
ServerInstance::ServerInstance(int32_t listening_socket_fd)
        : listening_socket_fd(listening_socket_fd) {}
