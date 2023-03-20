#include "HttpSession.h"
#include "Exception.h"
#include "SessionManager.h"

const std::string& ToString(HttpSessionState state) {
    static std::unordered_map<HttpSessionState, std::string> kStateToString = {
            {HttpSessionState::ReadRequest,   "ReadRequest"},
            {HttpSessionState::HandleRequest, "HandleRequest"},
            {HttpSessionState::ProcessFile,   "ProcessFile"},
            {HttpSessionState::ProcessCgi,    "ProcessCgi"},
    };
    return kStateToString[state];
}

std::shared_ptr<HttpSession> HttpSession::CreateAsPtr(
        const std::shared_ptr<Config>& config,
        const std::shared_ptr<EndpointConfig>& endpoint_config,
        boost::asio::io_context& io_context,
        boost::asio::ssl::context& ssl_context,
        boost::asio::ip::tcp::socket& tcp_socket,
        SessionManager& session_manager) {
    return std::shared_ptr<HttpSession>(
            new HttpSession(config, endpoint_config, io_context, ssl_context, tcp_socket, session_manager));
}

HttpSession::HttpSession(const std::shared_ptr<Config>& config,
                         const std::shared_ptr<EndpointConfig>& endpoint_config,
                         boost::asio::io_context& io_context,
                         boost::asio::ssl::context& ssl_context,
                         boost::asio::ip::tcp::socket& tcp_socket,
                         SessionManager& session_manager)
        :
        _config(config),
        _endpoint_config(endpoint_config),
        _server_config(std::nullopt),
        _keep_alive(false),
        _keep_alive_timeout(endpoint_config->GetDefaultServer()->default_keep_alive_timeout_s),
        _session_manager(session_manager),
        _ssl_socket(std::move(tcp_socket), ssl_context),
        _killer_timer(io_context, boost::posix_time::seconds(_keep_alive_timeout)),
        _request_parser(endpoint_config),
        _state(HttpSessionState::ReadRequest),
        id(GenId()) {
    LOG_DEBUG("HttpSessionConstructor, id: ", id);
}

void HttpSession::HandleReadRequest(const boost::system::error_code& error,
                                    std::size_t bytes_transferred) {
    LOG_DEBUG("HandleReadRequest, id: ", id);
    if (!error) {
        if (_state != HttpSessionState::ReadRequest) {
            LOG_WARNING("Read on wrong session state: \"", ToString(_state), "\"");
            return;
        }

        try {
            Http::RequestParseResult parse_result = _request_parser.Parse(
                    std::string_view(_buffer.data(), bytes_transferred));

            switch (parse_result.status) {

                case Http::RequestParseStatus::WaitMoreData: {
                    AsyncReadRequest();
                    break;
                }

                case Http::RequestParseStatus::Finish: {
                    LOG_IMPORTANT("Raw request on id: ", id, "\n", parse_result.request.value()->raw_request);

                    _state = HttpSessionState::HandleRequest;
                    _server_config = parse_result.request.value()->server_config;

                    Http::RequestHandler handler(parse_result.request.value());
                    Http::HandleResult handle_result = handler.Handle();

                    _keep_alive = handle_result.keep_alive;
                    _keep_alive_timeout = handle_result.keep_alive_timeout;

                    AsyncWriteResponse(handle_result.response);
                }
            }
        }
        catch (const Http::Exception& e) {
            AsyncWriteResponse(e.GetErrorResponse());
            LOG_INFO("Exception: ", e.what());
            _state = HttpSessionState::HandleRequest;
            _request_parser.Reset();
        }
        catch (const std::exception& e) {
            LOG_ERROR("Unexpected exception: ", e.what());
            AsyncWriteResponse(
                    Http::Response::MakeDefaultWithBody(
                            _endpoint_config->GetDefaultServer(),
                            Http::Code::InternalServerError, ToString(Http::Code::InternalServerError),
                            GetErrorPageByCode(Http::Code::InternalServerError, _endpoint_config->GetDefaultServer())));
            _state = HttpSessionState::HandleRequest;
            _request_parser.Reset();
        }
    }
    else if (error == boost::asio::error::operation_aborted || error == boost::asio::ssl::error::stream_truncated) {
        LOG_DEBUG("Ignored error, \"", error.message(), "\" on HandleReadRequest");
    }
    else {
        LOG_WARNING("Error, \"", error.message(), "\" on HandleReadRequest");
        Close();
    }
}

void HttpSession::HandleKillByTimeout(const boost::system::error_code& error) {
    LOG_DEBUG("HandleKillByTimeout, id: ", id);
    if (error != boost::asio::error::operation_aborted) {
        LOG_WARNING("Error on HandleKillByTimeout");
    }
    Close();
}

void HttpSession::HandleWriteResponse(const boost::system::error_code& error, size_t bytes_transferred) {
    LOG_DEBUG("HandleWriteResponse, id: ", id);
    std::ignore = bytes_transferred;
    if (!error) {
        if (!_keep_alive) {
            Close();
            return;
        }
        else {
            AsyncWaitKillByTimeout();
        }
        _state = HttpSessionState::ReadRequest;
        AsyncReadRequest();
    }
    else if (error == boost::asio::error::operation_aborted) {
        LOG_WARNING("Error on HandleWriteResponse");
        Close();
    }
    else {

    }
}

void HttpSession::HandleHandshake(const boost::system::error_code& error) {
    LOG_INFO("HandleHandshake, id: ", id);
    if (!error) {
        AsyncReadRequest();
    }
    else if (error == boost::asio::ssl::error::stream_truncated) {
        LOG_DEBUG("Ignored error: ", error.to_string(), "; Error message: ", error.message());
    }
    else {
        LOG_ERROR("Error: ", error.to_string(), "; error message: ", error.message());
        Close();
    }
}

void HttpSession::AsyncHandshake() {
    LOG_DEBUG("AsyncHandshake, id: ", id);
    auto handshake_lambda = [this_shared_ptr = shared_from_this()](const boost::system::error_code& error) {
        this_shared_ptr->HandleHandshake(error);
    };
    _ssl_socket.async_handshake(boost::asio::ssl::stream_base::server, handshake_lambda);
}

void HttpSession::AsyncWaitKillByTimeout() {
    LOG_DEBUG("AsyncWaitKillByTimeout, id: ", id);
    _killer_timer.cancel();

    auto kill_lambda = [this_shared_ptr = shared_from_this()](const boost::system::error_code& error_code) {
        this_shared_ptr->HandleKillByTimeout(error_code);
    };

    _killer_timer.expires_from_now(boost::posix_time::seconds(_keep_alive_timeout));
    _killer_timer.async_wait(kill_lambda);
}

void HttpSession::AsyncWriteResponse(const std::shared_ptr<Http::Response>& response) {
    LOG_DEBUG("AsyncWriteResponse, id: ", id);
    auto write_lambda = [this_shared_ptr = shared_from_this()](const boost::system::error_code& error_code,
                                                               size_t bytes_transferred) {
        this_shared_ptr->HandleWriteResponse(error_code, bytes_transferred);
    };

    _raw_response = response->Extract();
    boost::asio::async_write(_ssl_socket, boost::asio::buffer(_raw_response), write_lambda);
}

void HttpSession::AsyncReadRequest() {
    LOG_DEBUG("AsyncReadRequest, id: ", id);
    auto read_lambda = [this_shared_ptr = shared_from_this()](const boost::system::error_code& error_code,
                                                              std::size_t bytes_transferred) {
        this_shared_ptr->HandleReadRequest(error_code, bytes_transferred);
    };
    _ssl_socket.async_read_some(boost::asio::buffer(_buffer), read_lambda);
}

void HttpSession::Close() {
    LOG_DEBUG("Close, id: ", id);
    _session_manager.CloseSession(shared_from_this());
    _ssl_socket.lowest_layer().close();
    _killer_timer.cancel();
}