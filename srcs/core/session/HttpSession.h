#pragma once

#include "Response.h"
#include "Request.h"
#include "SharedPtr.h"

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
class HttpSession {
public:
    HttpSession(int session_id, const SharedPtr<ServerConfig>& server_instance, CoreModule* core_module)
            : server_instance_info(server_instance),
              request(MakeShared(new Request(server_instance))),
              available(true),
              keep_alive(true),
              keep_alive_timeout(server_instance_info->default_keep_alive_timeout),
              started_time(time(nullptr)),
              state(ConnectionState::HandleRequest),
              _core_module(core_module),
              _session_id(session_id) {}


    void SendDataToClient(const SharedPtr<Response>& processed_response, bool should_keep_alive);

    void Close();

    SharedPtr<ServerConfig> server_instance_info;

    SharedPtr<Request> request;

    SharedPtr<Response> response;

    bool available;

    bool keep_alive;

    int keep_alive_timeout;

    time_t started_time;

    ConnectionState::State state;

private:
    CoreModule* _core_module;

    int _session_id;
};

template<class CoreModule>
void HttpSession<CoreModule>::Close() {
    _core_module->CloseSession(_session_id);
}

template<class CoreModule>
void HttpSession<CoreModule>::SendDataToClient(const SharedPtr<Response>& processed_response, bool should_keep_alive) {
    this->response = processed_response;
    this->keep_alive = should_keep_alive;
    state = ConnectionState::ResponseToClient;
    _core_module->SendDataToClient(_session_id);
}

