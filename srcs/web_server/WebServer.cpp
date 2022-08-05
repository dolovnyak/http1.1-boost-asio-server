#include "WebServer.h"

#include <sys/socket.h>

void WebServer::Setup(const Config& config) {
    _config = config;

    /// for each server in config:
    ///     create non-blocking socket
    ///     create address
    ///     bind socket to address
    ///     listen socket
    ///     add socket to poll/epoll
}
