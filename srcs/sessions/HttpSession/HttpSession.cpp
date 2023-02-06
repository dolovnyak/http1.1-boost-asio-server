#include "HttpSession.h"

const std::string& ToString(HttpSessionState state) {
    static std::unordered_map<HttpSessionState, std::string> kStateToString = {
            {HttpSessionState::ReadRequest,      "ReadRequest"},
            {HttpSessionState::ProcessRequest,   "ProcessRequest"},
            {HttpSessionState::ProcessResource,  "ProcessResource"},
            {HttpSessionState::ResponseToClient, "ResponseToClient"}
    };
    return kStateToString[state];
}

std::shared_ptr<HttpSession> HttpSession::CreateAsPtr(
        const std::shared_ptr<Config>& config,
        const std::shared_ptr<EndpointConfig>& endpoint_config,
        boost::asio::io_context& io_context) {
    return std::shared_ptr<HttpSession>(new HttpSession(config, endpoint_config, io_context));
}

HttpSession::HttpSession(const std::shared_ptr<Config>& config,
                         const std::shared_ptr<EndpointConfig>& endpoint_config,
                         boost::asio::io_context& io_context)
        : _endpoint_config(endpoint_config),
          _server_config(std::nullopt),
          _socket(io_context),
          _request_parser(endpoint_config),
          _keep_alive(false),
          _keep_alive_timeout(endpoint_config->GetDefaultServer()->default_keep_alive_timeout_s),
          _state(HttpSessionState::ReadRequest) {}

void HttpSession::HandleRead(const boost::system::error_code& e,
                             std::size_t bytes_transferred) {

}

boost::asio::ip::tcp::socket& HttpSession::GetSocketAsReference() {
    return _socket;
}
