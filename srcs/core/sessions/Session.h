#pragma once

#include "Event.h"
#include "Logging.h"
#include "core_helpers.h"

#include <queue>

namespace SessionType {
    enum Type {
        Server,
        Http,
        File,
    };
}

class Session {
public:
    typedef typename std::unordered_map<SocketFd, std::shared_ptr<Session<CoreModule> > >::iterator It;

    typedef std::shared_ptr<Session<CoreModule> > Ptr;

public:
    Session(const std::shared_ptr<Config>& config, int core_module_index, CoreModule* core_module, SocketFd socket)
            : available(true),
              last_activity_time(time(nullptr)),
              config(config),
              core_module_index(core_module_index),
              core_module(core_module),
              socket(socket) {}

    virtual ~Session() {}

    virtual bool ShouldCloseAfterResponse() const = 0;

    virtual const std::string& GetResponseData() const = 0;

    virtual const std::string& GetName() const = 0;

    virtual SessionType::Type GetType() const = 0;

    void Close() {
        this->core_module->CloseSession(core_module_index);
    }

    void UpdateLastActivityTime();

public:
    bool available;
    time_t last_activity_time;
    std::shared_ptr<Config> config;
    int core_module_index;
    CoreModule* core_module;
    SocketFd socket;
};


void LogSession(Event* event, std::shared_ptr<Session<CoreModule> > session) {
    LOG_INFO(event->GetName(), " on ", session->GetName(), "_fd_", session->socket);
}

void LogSession(std::shared_ptr<Session<CoreModule> > session, const std::string& message) {
    LOG_INFO(session->GetName(), "_fd_", session->socket, ": ", message);
}

void Session<CoreModule>::UpdateLastActivityTime() {
    last_activity_time = time(nullptr);
}