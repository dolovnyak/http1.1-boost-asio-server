#pragma once

#include <queue>

#include "Config.h"
#include "Event.h"
#include "Logging.h"

template <class CoreModule>
class WebServer {
public:
    WebServer(const SharedPtr<Config>& config);

    void Run();

private:
    void ProcessCoreEvents();
    void ProcessEvents();

private:
    SharedPtr<Config> _config;
    CoreModule _core_module;
    std::queue<SharedPtr<Event> > _event_queue;
};

template <class CoreModule>
WebServer<CoreModule>::WebServer(const SharedPtr<Config>& config)
: _config(config), _core_module(config, &_event_queue) {}

template <class CoreModule>
void WebServer<CoreModule>::Run() {
    while (true) {
        /// TODO maybe if event_queue is empty set timeout to -1 (infinity) and if not - set timeout to 0
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