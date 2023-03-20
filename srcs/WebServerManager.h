#pragma once

#include "Config.h"
#include "EndpointSession.h"
#include "Http.h"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <queue>
#include <unordered_map>

class WebServerManager {
private:
    std::shared_ptr<Config> _config;
    boost::asio::io_context _io_context;
    std::vector<std::shared_ptr<EndpointSession>> _endpoint_sessions;
    boost::asio::ssl::context _ssl_context;

public:
    WebServerManager(const std::shared_ptr<Config>& config)
            : _config(config), _io_context(), _ssl_context(boost::asio::ssl::context::sslv23) {

        _ssl_context.set_options(boost::asio::ssl::context::default_workarounds
                                 | boost::asio::ssl::context::no_sslv2
                                 | boost::asio::ssl::context::single_dh_use);
        _ssl_context.use_certificate_chain_file("/etc/letsencrypt/live/dolov.dev/fullchain.pem");
        _ssl_context.use_private_key_file("/etc/letsencrypt/live/dolov.dev/privkey.pem", boost::asio::ssl::context::pem);
        _ssl_context.use_tmp_dh_file("dh2048.pem");

        for (auto& endpoint_config: _config->endpoint_configs) {
            auto endpoint_session = EndpointSession::CreateAsPtr(config, endpoint_config, _io_context, _ssl_context);
            _endpoint_sessions.emplace_back(endpoint_session);

            /// start accepting new connections (separated with constructor because of enable_shared_from_this restrictions
            endpoint_session->AsyncAccept();
        }

        if (_endpoint_sessions.empty()) {
            throw std::runtime_error("No server sessions created");
        }
    }

    void Run() {
        _io_context.run();
    }
};
