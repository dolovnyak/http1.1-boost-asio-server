#include <utility>

#include "Response.h"

#pragma once

class Connection {
public:
    Connection(std::shared_ptr<ServerInstance> server_instance,
               std::function<void()> set_write_event_to_core_module)
            :
            _server_instance(std::move(server_instance)),
            _set_write_event_to_core_module(std::move(set_write_event_to_core_module)),
            _still_available(true) {}

    void SetWriteEventToCoreModule() {
        if (_still_available) {
            _set_write_event_to_core_module();
        }
    }

    void SetUnavailable() {
        _still_available = false;
    }

    void SetResponse(std::string response_body) {
        _response_body = response_body;
    }

    std::string GetResponse() {
        return _response_body;
    }

private:
    /// connection should contain info about server which owns this connection
    std::shared_ptr<ServerInstance> _server_instance;

    std::function<void()> _set_write_event_to_core_module;

    std::string _response_body;

    bool _still_available;
};