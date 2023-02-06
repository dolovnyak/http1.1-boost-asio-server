#include "EndpointSession.h"

std::shared_ptr<EndpointSession> EndpointSession::CreateAsPtr(
        const std::shared_ptr<Config>& config,
        const std::shared_ptr<EndpointConfig>& endpoint_config,
        boost::asio::io_context& io_context) {
    return std::shared_ptr<EndpointSession>(new EndpointSession(config, endpoint_config, io_context));
}

EndpointSession::EndpointSession(const std::shared_ptr<Config>& config,
                                 const std::shared_ptr<EndpointConfig>& endpoint_config,
                                 boost::asio::io_context& io_context)
        : _config(config),
          _endpoint_config(endpoint_config),
          _io_context(io_context),
          _acceptor(io_context),
          _endpoint(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(endpoint_config->host),
                                                   endpoint_config->port)) {

    _acceptor.open(_endpoint.protocol());
    _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(_endpoint);
    _acceptor.listen();
    AsyncAccept(HttpSession::CreateAsPtr(config, endpoint_config, io_context));
}

void
EndpointSession::HandleAccept(const std::shared_ptr<HttpSession>& http_session, const boost::system::error_code& err) {
    /// TODO start reading on http_session
    LOG_DEBUG("Accept new");
    AsyncAccept(HttpSession::CreateAsPtr(_config, _endpoint_config, _io_context));
}

void EndpointSession::AsyncAccept(const std::shared_ptr<HttpSession>& http_session) {
    auto accept_lambda = [http_session, endpoint_session = this->shared_from_this()](auto&& PH1) {
        return endpoint_session->HandleAccept(http_session, std::forward<decltype(PH1)>(PH1));
    };
    _acceptor.async_accept(http_session->GetSocketAsReference(), accept_lambda);
}