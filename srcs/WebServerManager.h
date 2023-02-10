#pragma once

#include "Config.h"
#include "EndpointSession.h"
#include "Http.h"

#include <boost/asio.hpp>
#include <queue>
#include <unordered_map>

class WebServerManager {
private:
    std::shared_ptr<Config> _config;
    boost::asio::io_context _io_context;
    std::vector<std::shared_ptr<EndpointSession>> _endpoint_sessions;

public:
    WebServerManager(const std::shared_ptr<Config>& config)
            : _config(config), _io_context() {

        for (auto& endpoint_config: _config->endpoint_configs) {
            auto endpoint_session = EndpointSession::CreateAsPtr(config, endpoint_config, _io_context);
            _endpoint_sessions.emplace_back(endpoint_session);

            /// start accepting new connections (separated with constructor because of enable_shared_from_this restrictions
            endpoint_session->AsyncAccept(HttpSession::CreateAsPtr(config, endpoint_config, _io_context));
        }

        if (_endpoint_sessions.empty()) {
            throw std::runtime_error("No server sessions created");
        }
    }

    void Run() {
        _io_context.run();
    }
};
