#pragma once

#include "Config.h"
#include "Event.h"
#include "ServerSession.h"
#include "SessionsKillerEvent.h"
#include "Http.h"

#include <boost/asio.hpp>
#include <queue>
#include <unordered_map>

class WebServer {
public:
    WebServer(const std::shared_ptr<Config>& config)
            : _config(config) {


        for (PortServersIt it = _config->port_servers_configs.begin();
             it != _config->port_servers_configs.end(); ++it) {
            int socket = SetupServerSocket(it->second.port, _config);

            std::shared_ptr<Session> server_session = MakeShared<Session>(
                    new ServerSession<CoreModule>(
                            _core_module.GetNextSessionIndex(), &_core_module, SocketFd(socket), it->second));

            _core_module.AddSession(socket, server_session);
        }

        if (_sessions.empty()) {
            throw std::runtime_error("No server sessions created");
        }
    }

    void Run() {
        _io_service.run();
    }

private:
    std::shared_ptr<Config> _config;
    std::unordered_map<SocketFd, std::shared_ptr<Session>> _sessions;

    boost::asio::io_service _io_service;
    boost::asio::ip::tcp::acceptor _acceptor;
};
