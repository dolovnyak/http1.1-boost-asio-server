#pragma once

#include "Response.h"
#include "Request.h"
#include "SharedPtr.h"
#include "ServerInstance.h"

#include <utility>
#include <optional>

namespace HandleIncomingDataStatus {
    enum Status {
        NotAvailable = 0,
        Finish,
        WaitMoreData,
    };
}

namespace ConnectionState {
    enum State {
        HandleRequest = 0,
        ProcessRequest,
        ResponseToClient
    };
}

class HttpConnection {
public:
    HttpConnection(int fd, const SharedPtr<ServerInstance>& server_instance)
            :
            fd(fd),
            server_instance(server_instance),
            request(MakeShared<Request>(Request())),
            _available(true) {}

    HandleIncomingDataStatus::Status HandleIncomingData(const SharedPtr<std::string>& incoming_data);

    void ProcessIncomingData();

    void SendProcessedDataToClient();

    void SendErrorDataToClient(const SharedPtr<Response>& response);

    int fd;

    SharedPtr<ServerInstance> server_instance;

    SharedPtr<Request> request;

    SharedPtr<Response> response;

private:
    ConnectionState::State _state;

    bool _available;
};