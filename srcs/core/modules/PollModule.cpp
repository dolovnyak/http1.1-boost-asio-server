#include "PollModule.h"
#include "Logging.h"

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

bool PollModule::Setup(const Config& config, std::queue<SharedPtr<Event> >* event_queue) {
    _event_queue = event_queue;

    _poll_fds_number = 0;
    int option_value = 1;

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

        struct sockaddr_in address = {};
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

        _poll_fds[_poll_fds_number].fd = listening_socket_fd;
        _poll_fds[_poll_fds_number].events = POLLIN;
        ++_poll_fds_number;

        SharedPtr<ServerInstance> server_instance = SharedPtr<ServerInstance>::MakeShared(
                ServerInstance(listening_socket_fd, server_config.name));
        _servers.insert(std::pair<int, SharedPtr<ServerInstance> >(listening_socket_fd, server_instance));
    }

    if (_servers.empty()) {
        LOG_ERROR("No server instances created");
        return false;
    }

    return true;
}

void PollModule::ProcessEvents(int timeout) {
    LOG_DEBUG("Before PollModule, available sockets num: ", _poll_fds_number);
    if (poll(_poll_fds, _poll_fds_number, timeout) < 0) {
        LOG_PERROR("Failed to poll");
        return;
    }
    LOG_DEBUG("After PollModule");

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
                LOG_INFO("Connection closed: ", _poll_fds[i].fd);
                CloseConnection(i);
            }
            else {
                LOG_ERROR("Incorrect revents value: ", _poll_fds[i].revents);
                return;
            }
        }
    }

    ProcessCompress();
}

void PollModule::ProcessNewConnection(int index) {
    LOG_INFO("New connections processing");
    for (;;) {
        int new_connection_fd = accept(_poll_fds[index].fd, nullptr, nullptr); // TODO maybe fill address from this
        if (new_connection_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                LOG_INFO("FinishWithSuccess processing new connections");
                break;
            }
            else {
                LOG_PERROR("Failed to accept new connection");
                break;
            }
        }
        LOG_INFO("New connection: ", new_connection_fd);
        _poll_fds[_poll_fds_number].fd = new_connection_fd;
        _poll_fds[_poll_fds_number].events = POLLIN;
        struct pollfd* new_connection_pollfd_ptr = &_poll_fds[_poll_fds_number];
        ++_poll_fds_number;

        SharedPtr<Connection> connection = SharedPtr<Connection>::MakeShared(
                Connection(new_connection_fd, _servers.at(_poll_fds[index].fd)));
        _connections.insert(std::pair<int, SharedPtr<Connection> >(new_connection_fd, connection));
    }
}

void PollModule::ProcessRead(int index) {
    LOG_INFO("Read processing");
    char buffer[8];

    LOG_DEBUG("Reading...");
    ssize_t bytes_read = recv(_poll_fds[index].fd, buffer, sizeof(buffer), 0);
    std::shared_ptr<std::string> raw_request = std::make_shared<std::string>(buffer, bytes_read);
    LOG_DEBUG("Bytes read: ", bytes_read);

    if (bytes_read < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            LOG_WARNING("Something strange in ProcessRead");
            /// TODO make parse event and push it to queue
        }
        else {
            LOG_PERROR("Failed to read from socket");
            CloseConnection(index);
        }
    }
    else if (bytes_read == 0) {
        LOG_INFO("Connection closed by client: ", _poll_fds[index].fd);
        CloseConnection(index);
    }
    else {
        /// TODO make parse event and push it to queue
    }
}

void PollModule::ProcessWrite(int index) {
    LOG_INFO("Write processing");
    std::string response = _connections.at(_poll_fds[index].fd)->response.body;
    ssize_t bytes_send = send(_poll_fds[index].fd, response.c_str(), response.size(), 0);

    if (bytes_send < 0) {
        LOG_PERROR("Failed to send data");
    }
    LOG_INFO("Bytes send: ", bytes_send);

    CloseConnection(index);
}

void PollModule::CloseConnection(int index) {
    _connections.at(_poll_fds[index].fd)->still_available = false; /// Needed for events check is connection still available before change it
    _connections.erase(_poll_fds[index].fd);

    close(_poll_fds[index].fd);
    _poll_fds[index].fd = -1;
    _should_compress = true;
}

void PollModule::ProcessCompress() {
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
