#pragma once

#include "Config.h"
#include "utilities/log.h"

class ServerInstance {
public:
    ServerInstance(int32_t listen_socket_fd);

    int32_t listening_socket_fd;
    /// maybe it will be contain name, root path and cgi directory paths
};

class WebServer {
public:
    bool Setup(const Config& config);
    void Run();

private:
    Config _config;
    std::vector<ServerInstance> _web_server_instances;

    int _poll_fds_number;
    struct pollfd* _poll_fds;

    void PollProcessing(int timeout);
};