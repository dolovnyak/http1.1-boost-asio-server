#pragma once

#include "Config.h"
#include "Event.h"
#include "Logging.h"
#include "Session.h"

#include <queue>
#include <unordered_map>

template<class CoreModule>
class WebServer {
public:
    WebServer(const SharedPtr<Config>& config);

    void Run();

private:
    void ProcessCoreEvents();

    void ProcessEvents();

private:
    SharedPtr<Config> _config;
    std::queue<SharedPtr<Event> > _event_queue;
    std::unordered_map<int, SharedPtr<Session<CoreModule> > > _sessions;
    CoreModule _core_module;
};

template<class CoreModule>
WebServer<CoreModule>::WebServer(const SharedPtr<Config>& config)
        : _config(config),
          _core_module(config, &_event_queue, &_sessions) {}

template<class CoreModule>
void WebServer<CoreModule>::Run() {
    while (true) {
        try {
            ProcessCoreEvents();
            ProcessEvents();
        }
        catch (const std::exception& e) {
            LOG_ERROR("Exception in main loop: %s", e.what());
        }
    }
}

template<class CoreModule>
void WebServer<CoreModule>::ProcessCoreEvents() {
    if (_event_queue.empty()) {
        _core_module.ProcessEvents(-1);
    }
    else {
        _core_module.ProcessEvents(0);
    }
}

template<class CoreModule>
void WebServer<CoreModule>::ProcessEvents() {
    /// events which spawned by current events will processed in next iteration
    size_t event_queue_size = _event_queue.size();
    for (size_t i = 0; i < event_queue_size; ++i) {
        _event_queue.front()->Process();
        _event_queue.pop();
    }
}