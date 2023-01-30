#include "core_helpers.h"
#include "utilities.h"

#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>

int SetupServerSocket(int port, std::shared_ptr<Config> config) {
    int option_value = 1;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd == -1) {
        throw std::runtime_error("Failed to create listening socket");
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&option_value, sizeof(option_value)) < 0) {
        close(socket_fd);
        throw std::runtime_error("Failed to set socket options");
    }

    if (!SetSocketNonBlocking(socket_fd)) {
        close(socket_fd);
        throw std::runtime_error("Failed to set socket non blocking");
    }

    struct sockaddr_in address = {};
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(socket_fd);
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(socket_fd, config->max_sessions_number) < 0) {
        close(socket_fd);
        throw std::runtime_error("Failed to set listen backlog");
    }

    return socket_fd;
}

