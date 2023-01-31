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
private:
    std::shared_ptr<Config> _config;
    std::unordered_map<boost::asio::ip::tcp::socket, std::shared_ptr<Session>> _sessions;

    boost::asio::io_service _io_service;
    boost::asio::ip::tcp::acceptor _acceptor;

public:
    WebServer(const std::shared_ptr<Config>& config)
            : _config(config), _io_service(), _acceptor(_io_service) {


        for (auto & port_servers_config : _config->port_servers_configs) {
            int socket = SetupServerSocket(port_servers_config.second.port, _config);

            std::shared_ptr<Session> server_session = MakeShared<Session>(
                    new ServerSession(_core_module.GetNextSessionIndex(), &_core_module, boost::asio::ip::tcp::socket(socket), it->second));

            _core_module.AddSession(socket, server_session);
        }

        if (_sessions.empty()) {
            throw std::runtime_error("No server sessions created");
        }
    }

    void Run() {
        _io_service.run();
    }
};
