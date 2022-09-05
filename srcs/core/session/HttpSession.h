#pragma once

#include "Response.h"
#include "Request.h"
#include "SharedPtr.h"
#include "Session.h"
#include "utilities.h"

#include <utility>
#include <optional>

namespace ConnectionState {
    enum State {
        ReadRequest = 0,
        ProcessRequest,
        ProcessResource,
        ResponseToClient
    };
}

template<class CoreModule>
class HttpSession : public Session<CoreModule> {
public:
    HttpSession(int core_module_index, CoreModule* core_module, SocketFd socket,
                SharedPtr<ServerConfig> server_config)
            : Session<CoreModule>(core_module_index, core_module, socket),
              server_config(server_config),
              request(MakeShared(new Request(server_config))),
              keep_alive(true),
              keep_alive_timeout(server_config->default_keep_alive_timeout),
              started_time(time(nullptr)),
              state(ConnectionState::ReadRequest) {}

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
    SharedPtr<ServerConfig> server_config;

    SharedPtr<Request> request;

    std::string response;

    bool keep_alive;

    int keep_alive_timeout;

    time_t started_time;

    ConnectionState::State state;
};

template<class CoreModule>
void HttpSession<CoreModule>::SendDataToClient(const std::string& processed_response, bool should_keep_alive) {
    this->response = processed_response;
    this->keep_alive = should_keep_alive;
    state = ConnectionState::ResponseToClient;
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
    state = ConnectionState::ReadRequest;
}

template<class CoreModule>
const std::string& HttpSession<CoreModule>::GetName() const {
    static std::string kName = "HttpSession";
    return kName;
}
