#include "Response.h"
#include "Request.h"

#include <utility>
#include <optional>

#pragma once

class Connection {
public:
    Connection(int32_t fd,
               std::shared_ptr<ServerInstance> server_instance,
               std::function<void()> set_write_event_to_core_module)
            :
            fd(fd),
            server_instance(std::move(server_instance)),
            still_available(true),
            _set_write_event_to_core_module(std::move(set_write_event_to_core_module)) {}

    void SetWriteEventToCoreModule() {
        if (still_available) {
            _set_write_event_to_core_module();
        }
    }

    int32_t fd;

    Request request;

    Response response;

    std::shared_ptr<ServerInstance> server_instance;

    bool still_available;

private:

    /// connection should contain info about server which owns this connection

    std::function<void()> _set_write_event_to_core_module;



};