#pragma once

#include "Request.h"
#include "RequestParser.h"
#include "RequestHandler.h"
#include "Response.h"
#include "Session.h"
#include "utilities.h"

#include <utility>
#include <optional>

enum class HttpSessionState {
    ReadRequest = 0,
    HandleRequest,
    ProcessFile,
    ProcessCgi
};

const std::string& ToString(HttpSessionState state);

class SessionManager;

class HttpSession : public Session, public std::enable_shared_from_this<HttpSession> {
private:
    std::shared_ptr<Config> _config;

    std::shared_ptr<EndpointConfig> _endpoint_config;

    std::optional<std::shared_ptr<ServerConfig>> _server_config;

    boost::asio::io_context& _io_context;

    SessionManager& _session_manager;

    boost::asio::ip::tcp::socket _socket;

    Http::RequestParser _request_parser;

    bool _keep_alive;

    unsigned int _keep_alive_timeout;

    std::array<char, READ_BUFFER_SIZE> _buffer;

    HttpSessionState _state;

public:
    static std::shared_ptr<HttpSession> CreateAsPtr(
            const std::shared_ptr<Config>& config,
            const std::shared_ptr<EndpointConfig>& endpoint_config,
            boost::asio::io_context& io_context,
            SessionManager& session_manager);

    ~HttpSession() {
        LOG_INFO("Http session destroyed");
    }

    [[nodiscard]] boost::asio::ip::tcp::socket& GetSocketAsReference();

    void AsyncReadRequest();

    void AsyncWriteResponse(const std::shared_ptr<Http::Response>& response);

    void HandleReadRequest(const boost::system::error_code& error, std::size_t bytes_transferred);

    void HandleWriteResponse(const boost::system::error_code& error, size_t bytes_transferred);

    void HandleReadFile(const boost::system::error_code& error, std::size_t bytes_transferred);

    void Close();

    [[nodiscard]] const std::string& GetName() const override {
        static std::string kName = "HttpSession";
        return kName;
    }

private:
    HttpSession(const std::shared_ptr<Config>& config,
                const std::shared_ptr<EndpointConfig>& endpoint_config,
                boost::asio::io_context& io_context,
                SessionManager& session_manager);
};
