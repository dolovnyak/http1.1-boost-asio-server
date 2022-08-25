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
class Connection {
public:
    Connection(int id, const SharedPtr<ServerInfo>& server_instance, CoreModule* core_module)
            : server_instance(server_instance),
              request(MakeShared<Request>(Request(server_instance))),
              available(true),
              state(ConnectionState::HandleRequest),
              _core_module(core_module),
              _id(id) {}

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

    int _id;
};

template<class CoreModule>
void Connection<CoreModule>::Close() {
    _core_module->CloseConnection(_id);
}

template<class CoreModule>
void Connection<CoreModule>::SendProcessedDataToClient() {
    _core_module->SendDataToClient(_id);
}

template<class CoreModule>
void Connection<CoreModule>::SendErrorDataToClient(const SharedPtr<Response>& error_response) {
    response = error_response;
    should_close_after_send = true;
    _core_module->SendDataToClient(_id);
}
