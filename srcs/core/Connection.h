#include "Response.h"
#include "Request.h"
#include "SharedPtr.h"

#include <utility>
#include <optional>

#pragma once

class Connection {
public:
    Connection(int fd, const SharedPtr<ServerInstance>& server_instance)
            :
            fd(fd),
            server_instance(server_instance),
            request(MakeShared(Request())),
            still_available(true) {}


    int fd;

    SharedPtr<ServerInstance> server_instance;

    SharedPtr<Request> request;

    Response response;

    bool still_available;

private:

    /// connection should contain info about server which owns this connection

};