#include "HttpSession.h"
#include "HttpException.h"

#include <boost/bind.hpp>

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

void HttpSession::HandleRead(const boost::system::error_code& error,
                             std::size_t bytes_transferred) {
    if (!error) {
        if (!available) {
            LOG_INFO("Read on closed connection");
            return;
        }

        if (_state != HttpSessionState::ReadRequest) {
            LOG_INFO("Read on wrong session state");
            return;
        }

        try {
            RequestParseResult result = _request_parser.Parse(std::string_view(_buffer.data(), bytes_transferred));

            switch (result.status) {
                case RequestParseStatus::Finish: {
                    _state = HttpSessionState::ProcessRequest;
                    RequestHandler handler(result.request.value());
                    handler.Handle();
                }

                case RequestParseStatus::WaitMoreData:
                    AsyncRead();
                    return;
            }
        }
        catch (const HttpException& e) {
            AsyncWrite(e.GetErrorResponse());
            LOG_INFO("HttpException: ", e.what());
        }
        catch (const std::exception& e) {
            LOG_ERROR("Unexpected exception: ", e.what());
            AsyncWrite(Response::MakeErrorResponse(Http::Code::InternalServerError, "Internal server error",
                                                   _endpoint_config->GetDefaultServer()));
        }
    }
    else if (error != boost::asio::error::operation_aborted) {
        /// close session
    }
}

void HttpSession::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
        // Initiate graceful connection closure.
        boost::system::error_code ignored_ec;
        _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
        _socket.close();
    }

    if (error != boost::asio::error::operation_aborted) {
        /// close session
    }
}

void HttpSession::AsyncWrite(const std::shared_ptr<Response>& response) {
    auto write_lambda = [this_ptr = shared_from_this()](const boost::system::error_code& error_code,
                                                        size_t bytes_transferred) {
        this_ptr->HandleWrite(error_code, bytes_transferred);
    };

    /// TODO change std::string response on boost buffer
    boost::asio::async_write(_socket, boost::asio::buffer(response->response), write_lambda);
}

void HttpSession::AsyncRead() {
    auto read_lambda = [this_ptr = shared_from_this()](const boost::system::error_code& error_code,
                                                       std::size_t bytes_transferred) {
        this_ptr->HandleRead(error_code, bytes_transferred);
    };

    _socket.async_read_some(boost::asio::buffer(_buffer), read_lambda);
}

boost::asio::ip::tcp::socket& HttpSession::GetSocketAsReference() {
    return _socket;
}
