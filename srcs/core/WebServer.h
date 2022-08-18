#pragma once

#include <queue>

#include "Config.h"
#include "core/events/Event.h"
#include "ServerInstance.h"
#include "Logging.h"

template <class CoreModule>
class WebServer {
public:
    bool Setup(const Config& config);

    void Run();

private:
    void ProcessCoreEvents();
    void ProcessEvents();

private:
    Config _config;
    CoreModule _core_module;
    std::queue<SharedPtr<Event> > _event_queue;
};

template <class EventHandler>
bool WebServer<EventHandler>::Setup(const Config& config) {
    _config = config; /// TODO maybe remove

    if (!_core_module.Setup(config, &_event_queue)) {
        LOG_ERROR("Failed to setup core module");
        return false;
    }

    return true;
}

template <class CoreModule>
void WebServer<CoreModule>::Run() {
    while (true) {
        ProcessCoreEvents();
        ProcessEvents();
    }
}

template <class CoreModule>
void WebServer<CoreModule>::ProcessCoreEvents() {
    _core_module.ProcessEvents(-1);
}

template <class CoreModule>
void WebServer<CoreModule>::ProcessEvents() {
    while (!_event_queue.empty()) {
        _event_queue.front()->Process();
        _event_queue.pop();
    }
}