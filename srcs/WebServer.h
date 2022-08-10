#pragma once

#include <queue>

#include "Config.h"
#include "Event.h"
#include "ServerInstance.h"
#include "logging.h"

template <class CoreModule>
class WebServer {
public:
    bool Setup(const Config& config);

    [[noreturn]] void Run();

private:
    void ProcessCoreEvents();
    void ProcessEvents();

private:
    Config _config;
    CoreModule _core_module;
    std::vector<ServerInstance> _server_instances;
    std::queue<Event> _event_queue;
};

template <class EventHandler>
bool WebServer<EventHandler>::Setup(const Config& config) {
    _config = config;

    _server_instances = _core_module.Setup(config);
    if (_server_instances.empty()) {
        return false;
    }

    
    return true;
}

template <class EventHandler>
[[noreturn]] void WebServer<EventHandler>::Run() {
    while (true) {
        ProcessCoreEvents();
        ProcessEvents();
    }
}

template <class EventHandler>
void WebServer<EventHandler>::ProcessCoreEvents() {
    _core_module.ProcessEvents(-1);
}

template <class EventHandler>
void WebServer<EventHandler>::ProcessEvents() {
    while (!_event_queue.empty()) {
        Event event = _event_queue.front();
        _event_queue.pop();
        event.Process();
    }
}