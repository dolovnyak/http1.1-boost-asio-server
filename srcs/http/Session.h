#pragma once

#include "Response.h"
#include "Request.h"
#include "SharedPtr.h"
#include "ServerInfo.h"

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
class Session {
public:
    Session(int connection_id, const SharedPtr<ServerInfo>& server_instance, CoreModule* core_module)
            : server_instance(server_instance),
              request(MakeShared(new Request(server_instance))),
              available(true),
              state(ConnectionState::HandleRequest),
              _core_module(core_module),
              _connection_id(connection_id) {}

    void SendProcessedDataToClient();

    void SendErrorDataToClient(const SharedPtr<Response>& error_response);

    void Close();

    SharedPtr<ServerInfo> server_instance;

    SharedPtr<Request> request;

    SharedPtr<Response> response;

    bool available;

    bool should_close_after_send;

    ConnectionState::State state;

private:
    CoreModule* _core_module;

    int _connection_id;
};

template<class CoreModule>
void Session<CoreModule>::Close() {
    _core_module->CloseConnection(_connection_id);
}

template<class CoreModule>
void Session<CoreModule>::SendProcessedDataToClient() {
    _core_module->SendDataToClient(_connection_id);
}

template<class CoreModule>
void Session<CoreModule>::SendErrorDataToClient(const SharedPtr<Response>& error_response) {
    response = error_response;
    should_close_after_send = true;
    _core_module->SendDataToClient(_connection_id);
}

