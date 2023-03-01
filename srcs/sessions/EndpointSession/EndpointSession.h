#pragma once

#include "utilities.h"
#include "Config.h"
#include "SessionManager.h"

#include <memory>

class EndpointSession : public std::enable_shared_from_this<EndpointSession> {
private:
    std::shared_ptr<Config> _config;

    std::shared_ptr<EndpointConfig> _endpoint_config;

    boost::asio::io_context& _io_context;

    boost::asio::ip::tcp::acceptor _acceptor;

    boost::asio::ip::tcp::endpoint _endpoint;

public:
    SessionManager manager;

public:
    [[nodiscard]] static std::shared_ptr<EndpointSession> CreateAsPtr(
            const std::shared_ptr<Config>& config,
            const std::shared_ptr<EndpointConfig>& endpoint_config,
            boost::asio::io_context& io_context);

    void AsyncAccept(const std::shared_ptr<HttpSession>& http_session);

    void HandleAccept(const std::shared_ptr<HttpSession>& http_session, const boost::system::error_code& error);

    [[nodiscard]] const std::string& GetName() const {
        static std::string kName = "EndpointSession";
        return kName;
    }

private:
    EndpointSession(const std::shared_ptr<Config>& config,
                    const std::shared_ptr<EndpointConfig>& endpoint_config,
                    boost::asio::io_context& io_context);
};