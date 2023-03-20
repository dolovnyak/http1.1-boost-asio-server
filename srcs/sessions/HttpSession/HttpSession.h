#pragma once

#include "Request.h"
#include "RequestParser.h"
#include "RequestHandler.h"
#include "Response.h"
#include "utilities.h"
#include "Logger.h"

#include <utility>
#include <optional>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

enum class HttpSessionState {
    ReadRequest = 0,
    HandleRequest,
    ProcessFile,
    ProcessCgi
};

const std::string& ToString(HttpSessionState state);

class SessionManager;

class HttpSession : public std::enable_shared_from_this<HttpSession> {
private:
    std::shared_ptr<Config> _config;

    std::shared_ptr<EndpointConfig> _endpoint_config;

    std::optional<std::shared_ptr<ServerConfig>> _server_config;

    bool _keep_alive;

    unsigned int _keep_alive_timeout;

    SessionManager& _session_manager;

    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> _ssl_socket;

    boost::asio::deadline_timer _killer_timer;

    Http::RequestParser _request_parser;

    std::string _raw_response;

    std::array<char, READ_BUFFER_SIZE> _buffer;

    HttpSessionState _state;


    static int GenId() {
        static int id = 0;
        return ++id;
    }

public:
    int id;

    static std::shared_ptr<HttpSession> CreateAsPtr(
            const std::shared_ptr<Config>& config,
            const std::shared_ptr<EndpointConfig>& endpoint_config,
            boost::asio::io_context& io_context,
            boost::asio::ssl::context& ssl_context,
            boost::asio::ip::tcp::socket& tcp_socket,
            SessionManager& session_manager);

    void AsyncReadRequest();

    void AsyncHandshake();

    void AsyncWriteResponse(const std::shared_ptr<Http::Response>& response);

    void AsyncWaitKillByTimeout();

    void HandleReadRequest(const boost::system::error_code& error, std::size_t bytes_transferred);

    void HandleHandshake(const boost::system::error_code& error);

    void HandleWriteResponse(const boost::system::error_code& error, size_t bytes_transferred);

    void HandleKillByTimeout(const boost::system::error_code& error);

    void Close();

    [[nodiscard]] const std::string& GetName() const {
        static std::string kName = "HttpSession";
        return kName;
    }

private:
    HttpSession(const std::shared_ptr<Config>& config,
                const std::shared_ptr<EndpointConfig>& endpoint_config,
                boost::asio::io_context& io_context,
                boost::asio::ssl::context& ssl_context,
                boost::asio::ip::tcp::socket& tcp_socket,
                SessionManager& session_manager);
};
