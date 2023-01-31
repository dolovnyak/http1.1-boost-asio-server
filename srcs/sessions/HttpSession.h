#pragma once

#include "Response.h"
#include "Request.h"
#include "Session.h"
#include "utilities.h"

#include <utility>
#include <optional>

namespace HttpSessionState {
    enum State {
        ReadRequest = 0,
        ProcessRequest,
        ProcessResource,
        ResponseToClient
    };

    const std::string& ToString(State state);
}

class HttpSession : public Session {
public:
    HttpSession(const std::shared_ptr<Config>& config, boost::asio::ip::tcp::socket socket,
                const std::shared_ptr<PortServersConfig>& server_config_in)
            : Session(config, std::move(socket)),
              port_servers_config(server_config_in),
              request(std::make_shared<Request>(Request(port_servers_config->GetDefault()))),
              keep_alive(false),
              keep_alive_timeout(this->config->default_keep_alive_timeout_s),
              state(HttpSessionState::ReadRequest) {}

    ~HttpSession() {}

    [[nodiscard]] const std::string& GetResponseData() const override {
        return response;
    }

    [[nodiscard]] SessionType::Type GetType() const override {
        return SessionType::Http;
    }

    void SendDataToClient(const std::string& processed_response, bool should_keep_alive) {
        this->response = processed_response;
        this->keep_alive = should_keep_alive;
        state = HttpSessionState::ResponseToClient;
        /// TODO
    }

    [[nodiscard]] bool ShouldCloseAfterResponse() const override {
        return !keep_alive;
    }

    void Clear() {
        request->Clear();
        response.clear();
        state = HttpSessionState::ReadRequest;
    }

    [[nodiscard]] const std::string& GetName() const override {
        static std::string kName = "HttpSession";
        return kName;
    }

public:
    std::shared_ptr<PortServersConfig> port_servers_config;

    std::shared_ptr<Request> request;

    std::string response;

    bool keep_alive;

    int keep_alive_timeout;

    HttpSessionState::State state;
};
