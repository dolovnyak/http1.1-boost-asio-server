#pragma once

#include "utilities.h"
#include "Config.h"
#include "SessionManager.h"

#include <memory>
#include <boost/asio/ssl.hpp>

class EndpointSession : public std::enable_shared_from_this<EndpointSession> {
private:
    std::shared_ptr<Config> _config;

    std::shared_ptr<EndpointConfig> _endpoint_config;

    boost::asio::io_context& _io_context;

    boost::asio::ssl::context& _ssl_context;

    boost::asio::ip::tcp::acceptor _acceptor;

    boost::asio::ip::tcp::endpoint _endpoint;

public:
    SessionManager manager;

public:
    [[nodiscard]] static std::shared_ptr<EndpointSession> CreateAsPtr(
            const std::shared_ptr<Config>& config,
            const std::shared_ptr<EndpointConfig>& endpoint_config,
            boost::asio::io_context& io_context,
            boost::asio::ssl::context& ssl_context);

    void AsyncAccept();

    void HandleAccept(boost::asio::ip::tcp::socket& socket, const boost::system::error_code& error);

    [[nodiscard]] const std::string& GetName() const {
        static std::string kName = "EndpointSession";
        return kName;
    }

private:
    EndpointSession(const std::shared_ptr<Config>& config,
                    const std::shared_ptr<EndpointConfig>& endpoint_config,
                    boost::asio::io_context& io_context,
                    boost::asio::ssl::context& ssl_context);
};