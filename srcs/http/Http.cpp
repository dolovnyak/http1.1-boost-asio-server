#include "Http.h"
#include "utilities.h"

#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>

namespace Http {

    int SetupSocket(SharedPtr<ServerConfig> server_config, SharedPtr<Config> config) {
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
        address.sin_port = htons(server_config->port);

        if (bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            close(socket_fd);
            throw std::runtime_error("Failed to bind socket");
        }

        if (listen(socket_fd, config->max_sockets_number) < 0) {
            close(socket_fd);
            throw std::runtime_error("Failed to set listen backlog");
        }

        return socket_fd;
    }

    Method GetMethod(const std::string& method) {
        if (method == "GET") {
            return GET;
        }
        else if (method == "HEAD") {
            return HEAD;
        }
        else if (method == "POST") {
            return POST;
        }
        else if (method == "DELETE") {
            return DELETE;
        }
        else if (method == "PUT") {
            return PUT;
        }
        else if (method == "CONNECT") {
            return CONNECT;
        }
        else if (method == "OPTIONS") {
            return OPTIONS;
        }
        else if (method == "TRACE") {
            return TRACE;
        }
        else if (method == "PATCH") {
            return PATCH;
        }
        else {
            return UNKNOWN;
        }
    }

}