#include "WebServer.h"

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

bool WebServer::Setup(const Config& config) {
    _config = config;
    _poll_fd_number = 0;
    int option_value = 1;

    /// TODO i don't know will it work if I use std allocator, need to check
    struct pollfd* poll_fds;
    poll_fds = (pollfd*)calloc(config.GetMaxEventsNumber(), sizeof(struct pollfd));
    if (poll_fds == nullptr) {
        LOG_ERROR("Failed to allocate memory for poll_fds");
        return false;
    }

    for (const auto& server_config : config.GetServersConfigs()) {

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
        poll_fds[_poll_fd_number].fd = listening_socket_fd;
        poll_fds[_poll_fd_number].events = POLLIN;
        ++_poll_fd_number;

        ServerInstance server_instance(listening_socket_fd);
        _web_server_instances.push_back(server_instance);
    }

    return true;
}

void WebServer::Run() {

}


/// for now make only poll support without thoughts about epoll
ServerInstance::ServerInstance(int32_t listening_socket_fd)
        : listening_socket_fd(listening_socket_fd) {}
