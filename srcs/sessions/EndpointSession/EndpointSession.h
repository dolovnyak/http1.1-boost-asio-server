#pragma once

#include "utilities.h"
#include "Config.h"
#include "HttpSession.h"

#include <memory>

class EndpointSession : public Session, public std::enable_shared_from_this<EndpointSession> {
private:
    std::shared_ptr<Config> _config;
    std::shared_ptr<EndpointConfig> _endpoint_config;
    boost::asio::io_context& _io_context;
    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::ip::tcp::endpoint _endpoint;
    std::unordered_set<std::shared_ptr<Session>> _sessions;

public:
    [[nodiscard]] static std::shared_ptr<EndpointSession> CreateAsPtr(
            const std::shared_ptr<Config>& config,
            const std::shared_ptr<EndpointConfig>& endpoint_config,
            boost::asio::io_context& io_context);

    void AsyncAccept(const std::shared_ptr<HttpSession>& http_session);

    void HandleAccept(const std::shared_ptr<HttpSession>& http_session, const boost::system::error_code& error);

    [[nodiscard]] const std::string& GetName() const override {
        static std::string kName = "EndpointSession";
        return kName;
    }

    [[nodiscard]] SessionType::Type GetType() const override {
        return SessionType::Server;
    }

private:
    EndpointSession(const std::shared_ptr<Config>& config,
                    const std::shared_ptr<EndpointConfig>& endpoint_config,
                    boost::asio::io_context& io_context);
};