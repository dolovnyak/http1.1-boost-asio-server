#pragma once

class ServerInstance {
public:
    ServerInstance(int32_t listen_socket_fd) : listening_socket_fd(listen_socket_fd) {}

    int32_t listening_socket_fd;
    /// maybe it will be contain name, root path and cgi directory paths
};