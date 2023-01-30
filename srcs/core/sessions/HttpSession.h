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

template<class CoreModule>
class HttpSession : public Session<CoreModule> {
public:
    HttpSession(const std::shared_ptr<Config>& config, int core_module_index, CoreModule* core_module, SocketFd socket,
                const std::shared_ptr<PortServersConfig>& server_config_in)
            : Session<CoreModule>(config, core_module_index, core_module, socket),
              port_servers_config(server_config_in),
              request(std::make_shared<Request>(Request(port_servers_config->GetDefault()))),
              keep_alive(false),
              keep_alive_timeout(this->config->default_keep_alive_timeout_s),
              state(HttpSessionState::ReadRequest) {}

    ~HttpSession() {}

    const std::string& GetResponseData() const OVERRIDE;

    SessionType::Type GetType() const override {
        return SessionType::Http;
    }

    void SendDataToClient(const std::string& processed_response, bool should_keep_alive);

    bool ShouldCloseAfterResponse() const override;

    void Clear();

    const std::string& GetName() const override;

public:
    std::shared_ptr<PortServersConfig> port_servers_config;

    std::shared_ptr<Request> request;

    std::string response;

    bool keep_alive;

    int keep_alive_timeout;

    HttpSessionState::State state;
};

template<class CoreModule>
void HttpSession<CoreModule>::SendDataToClient(const std::string& processed_response, bool should_keep_alive) {
    this->response = processed_response;
    this->keep_alive = should_keep_alive;
    state = HttpSessionState::ResponseToClient;
    this->core_module->SendDataToSocket(this->core_module_index);
}

template<class CoreModule>
bool HttpSession<CoreModule>::ShouldCloseAfterResponse() const {
    return !keep_alive;
}

template<class CoreModule>
const std::string& HttpSession<CoreModule>::GetResponseData() const {
    return response;
}

template<class CoreModule>
void HttpSession<CoreModule>::Clear() {
    request->Clear();
    response.clear();
    state = HttpSessionState::ReadRequest;
}

template<class CoreModule>
const std::string& HttpSession<CoreModule>::GetName() const {
    static std::string kName = "HttpSession";
    return kName;
}
