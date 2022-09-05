#pragma once

#include "Config.h"
#include "Event.h"
#include "ServerSession.h"
#include "Http.h"

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
    std::unordered_map<SocketFd, SharedPtr<Session<CoreModule> > > _sessions;
    CoreModule _core_module;
};

template<class CoreModule>
WebServer<CoreModule>::WebServer(const SharedPtr<Config>& config)
        : _config(config),
          _core_module(config, &_event_queue, &_sessions) {

    for (size_t i = 0; i < _config->servers_configs.size(); ++i) {
        int socket = Http::SetupSocket(_config->servers_configs[i], _config);

        SharedPtr<Session<CoreModule> > server_session = MakeShared<Session<CoreModule> >(new ServerSession<CoreModule>(
                _core_module.GetNextSessionIndex(), &_core_module, SocketFd(socket), _config->servers_configs[i]));

        _core_module.AddSession(socket, server_session);
    }

    if (_sessions.empty()) {
        throw std::runtime_error("No server sessions created");
    }

}

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
        LOG_INFO("Event \"", _event_queue.front()->GetName(), "\"", " is processing");
        _event_queue.front()->Process();
        _event_queue.pop();
    }
}