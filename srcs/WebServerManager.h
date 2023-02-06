#pragma once

#include "Config.h"
#include "Event.h"
#include "sessions/EndpointSession/EndpointSession.h"
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
            _endpoint_sessions.emplace_back(EndpointSession::CreateAsPtr(config, endpoint_config, _io_context));
        }

        if (_endpoint_sessions.empty()) {
            throw std::runtime_error("No server sessions created");
        }
    }

    void Run() {
        _io_context.run();
    }
};
