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
        HandleRequest = 0,
        ProcessRequest,
        ResponseToClient
    };
}

template<class CoreModule>
class HttpSession : public Session<CoreModule> {
public:
    HttpSession(int session_id, const SharedPtr<ServerConfig>& server_instance, CoreModule* core_module)
            : Session<CoreModule>(session_id, core_module),
              server_config(server_instance),
              request(MakeShared(new Request(server_instance))),
              keep_alive(true),
              keep_alive_timeout(server_config->default_keep_alive_timeout),
              started_time(time(nullptr)),
              state(ConnectionState::HandleRequest) {}


    void SendDataToClient(const SharedPtr<Response>& processed_response, bool should_keep_alive);

    void Close();

    bool ShouldCloseAfterResponse() const override;

public:
    SharedPtr<ServerConfig> server_config;

    SharedPtr<Request> request;

    SharedPtr<Response> response;

    bool keep_alive;

    int keep_alive_timeout;

    time_t started_time;

    ConnectionState::State state;
};

template<class CoreModule>
void HttpSession<CoreModule>::Close() {
    this->_core_module->CloseSession(this->_session_id);
}

template<class CoreModule>
void HttpSession<CoreModule>::SendDataToClient(const SharedPtr<Response>& processed_response, bool should_keep_alive) {
    this->response = processed_response;
    this->keep_alive = should_keep_alive;
    state = ConnectionState::ResponseToClient;
    this->_core_module->SendDataToSocket(this->_session_id);
}

template<class CoreModule>
bool HttpSession<CoreModule>::ShouldCloseAfterResponse() const {
    return !keep_alive;
}
