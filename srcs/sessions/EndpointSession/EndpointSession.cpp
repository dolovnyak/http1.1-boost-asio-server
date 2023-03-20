#include "EndpointSession.h"

std::shared_ptr<EndpointSession> EndpointSession::CreateAsPtr(
        const std::shared_ptr<Config>& config,
        const std::shared_ptr<EndpointConfig>& endpoint_config,
        boost::asio::io_context& io_context,
        boost::asio::ssl::context& ssl_context) {
    return std::shared_ptr<EndpointSession>(new EndpointSession(config, endpoint_config, io_context, ssl_context));
}

EndpointSession::EndpointSession(const std::shared_ptr<Config>& config,
                                 const std::shared_ptr<EndpointConfig>& endpoint_config,
                                 boost::asio::io_context& io_context,
                                 boost::asio::ssl::context& ssl_context)
        : _config(config),
          _endpoint_config(endpoint_config),
          _io_context(io_context),
          _ssl_context(ssl_context),
          _acceptor(io_context),
          _endpoint(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(endpoint_config->host),
                                                   endpoint_config->port)) {
    LOG_DEBUG("EndpointSessionConstructor");
    _acceptor.open(_endpoint.protocol());
    _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(_endpoint);
    _acceptor.listen();
}

void
EndpointSession::HandleAccept(boost::asio::ip::tcp::socket& socket, const boost::system::error_code& error) {
    LOG_DEBUG("HandleAccept");
    if (!error) {
        LOG_DEBUG("Accept new https http_session");
        auto http_session = HttpSession::CreateAsPtr(_config, _endpoint_config, _io_context, _ssl_context, socket, manager);
        /// We can't do it inside constructor because we can't share from non fully constructed object
        http_session->AsyncWaitKillByTimeout();
        http_session->AsyncHandshake();
        manager.AddSession(http_session);
    }
    else {
        LOG_WARNING("Error, \"", error.message(), "\" on HandleAccept");
    }
    AsyncAccept();
}

void EndpointSession::AsyncAccept() {
    LOG_DEBUG("AsyncAccept");
    auto accept_lambda = [this_shared_ptr = shared_from_this()](const boost::system::error_code& err,
                                                         boost::asio::ip::tcp::socket socket) {
        this_shared_ptr->HandleAccept(socket, err);
    };
    _acceptor.async_accept(accept_lambda);
}
