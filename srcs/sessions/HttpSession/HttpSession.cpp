#include "HttpSession.h"
#include "Exception.h"

#include <boost/bind.hpp>

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
        boost::asio::io_context& io_context) {
    return std::shared_ptr<HttpSession>(new HttpSession(config, endpoint_config, io_context));
}

HttpSession::HttpSession(const std::shared_ptr<Config>& config,
                         const std::shared_ptr<EndpointConfig>& endpoint_config,
                         boost::asio::io_context& io_context)
        : _endpoint_config(endpoint_config),
          _server_config(std::nullopt),
          _io_context(io_context),
          _socket(io_context),
          _request_parser(endpoint_config),
          _keep_alive(false),
          _keep_alive_timeout(endpoint_config->GetDefaultServer()->default_keep_alive_timeout_s),
          _state(HttpSessionState::ReadRequest) {}

void HttpSession::HandleReadRequest(const boost::system::error_code& error,
                                    std::size_t bytes_transferred) {
    if (!error) {
        if (!available) {
            LOG_INFO("Read on closed connection");
            return;
        }

        if (_state != HttpSessionState::ReadRequest) {
            LOG_INFO("Read on wrong session state: \"", ToString(_state), "\"");
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
                    _state = HttpSessionState::HandleRequest;
                    _server_config = parse_result.request.value()->server_config;

                    Http::RequestHandler handler(parse_result.request.value());
                    Http::HandleResult handle_result = handler.Handle();

                    _keep_alive = handle_result.keep_alive;
                    /// TODO set keep alive timer

                    AsyncWriteResponse(handle_result.response);
                }
            }
        }
        catch (const Http::Exception& e) {
            AsyncWriteResponse(e.GetErrorResponse());
            LOG_INFO("Exception: ", e.what());
        }
        catch (const std::exception& e) {
            LOG_ERROR("Unexpected exception: ", e.what());
            AsyncWriteResponse(
                    Http::Response::MakeErrorResponse(Http::Code::InternalServerError, "Internal server error",
                                                      _endpoint_config->GetDefaultServer(), {}));
        }
    }
    else if (error != boost::asio::error::operation_aborted) {
        /// close session
    }
}

void HttpSession::HandleWriteResponse(const boost::system::error_code& error, size_t bytes_transferred) {
    LOG_INFO("HandleWriteResponse");
    if (!error) {
        if (!_keep_alive) {
            boost::system::error_code ignored_ec;
            _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
            _socket.close();
        }
        _state = HttpSessionState::ReadRequest;
        AsyncReadRequest();
    }
    else if (error != boost::asio::error::operation_aborted) {
        /// close session
    }
}

void HttpSession::AsyncWriteResponse(const std::shared_ptr<Http::Response>& response) {
    LOG_INFO("AsyncWriteResponse");
    auto write_lambda = [this_ptr = shared_from_this()](const boost::system::error_code& error_code,
                                                        size_t bytes_transferred) {
        this_ptr->HandleWriteResponse(error_code, bytes_transferred);
    };

    /// TODO change std::string response on boost buffer
    boost::asio::async_write(_socket, boost::asio::buffer(response->response), write_lambda);
}

void HttpSession::AsyncReadRequest() {
    LOG_INFO("AsyncReadRequest");
    auto read_lambda = [this_ptr = shared_from_this()](const boost::system::error_code& error_code,
                                                       std::size_t bytes_transferred) {
        this_ptr->HandleReadRequest(error_code, bytes_transferred);
    };
    _socket.async_read_some(boost::asio::buffer(_buffer), read_lambda);
}

boost::asio::ip::tcp::socket& HttpSession::GetSocketAsReference() {
    return _socket;
}
