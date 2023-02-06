#pragma once

#include "Request.h"
#include "RequestParser.h"
#include "Session.h"
#include "utilities.h"

#include <utility>
#include <optional>

enum class HttpSessionState {
    ReadRequest = 0,
    ProcessRequest,
    ProcessResource,
    ResponseToClient
};

const std::string& ToString(HttpSessionState state);

class HttpSession : public Session, public std::enable_shared_from_this<HttpSession> {
private:
    std::shared_ptr<Config> _config;

    std::shared_ptr<EndpointConfig> _endpoint_config;

    std::optional<std::shared_ptr<ServerConfig>> _server_config;

    boost::asio::ip::tcp::socket _socket;

    RequestParser _request_parser;

    bool _keep_alive;

    unsigned int _keep_alive_timeout;

    HttpSessionState _state; /// TODO maybe del

public:
    static std::shared_ptr<HttpSession> CreateAsPtr(
            const std::shared_ptr<Config>& config,
            const std::shared_ptr<EndpointConfig>& endpoint_config,
            boost::asio::io_context& io_context);

    [[nodiscard]] boost::asio::ip::tcp::socket& GetSocketAsReference();

    void HandleRead(const boost::system::error_code& e, std::size_t bytes_transferred);

    [[nodiscard]] SessionType::Type GetType() const override {
        return SessionType::Http;
    }

    [[nodiscard]] const std::string& GetName() const override {
        static std::string kName = "HttpSession";
        return kName;
    }

private:
    HttpSession(const std::shared_ptr<Config>& config,
                const std::shared_ptr<EndpointConfig>& endpoint_config,
                boost::asio::io_context& io_context);
};


//void SendDataToClient(const std::string& processed_response, bool should_keep_alive) {
////        this->response = processed_response;
//    this->_keep_alive = should_keep_alive;
//    _state = HttpSessionState::ResponseToClient;
//    /// TODO
//}
//
//void Clear() {
////        response.clear();
//    _state = HttpSessionState::ReadRequest;
//}
