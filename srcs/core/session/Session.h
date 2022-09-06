#pragma once

#include "Event.h"
#include "Logging.h"
#include "core_helpers.h"
#include "SharedPtr.h"

#include <queue>

namespace SessionType {
    enum Type {
        Server,
        Http,
        File,
    };
}

template<class CoreModule>
class Session {
public:
    typedef typename std::unordered_map<SocketFd, SharedPtr<Session<CoreModule> > >::iterator It;

    typedef SharedPtr<Session<CoreModule> > Ptr;

public:
    Session(int core_module_index, CoreModule* core_module, SocketFd socket)
            : available(true),
              last_activity_time(time(nullptr)),
              core_module_index(core_module_index),
              core_module(core_module),
              socket(socket) {}

    virtual ~Session() {}

    virtual bool ShouldCloseAfterResponse() const = 0;

    virtual const std::string& GetResponseData() const = 0;

    virtual const std::string& GetName() const = 0;

    virtual SessionType::Type GetType() const = 0;

    void Close();

    void UpdateLastActivityTime();

public:
    bool available;
    time_t last_activity_time;
    int core_module_index;
    CoreModule* core_module;
    SocketFd socket;
};

template<class CoreModule>
void Session<CoreModule>::Close() {
    this->core_module->CloseSession(core_module_index);
}

template<class CoreModule>
void LogSession(Event* event, SharedPtr<Session<CoreModule> > session) {
    LOG_INFO(event->GetName(), " on ", session->GetName(), "_fd_", session->socket);
}

template<class CoreModule>
void LogSession(SharedPtr<Session<CoreModule> > session, const std::string& message) {
    LOG_INFO(session->GetName(), "_fd_", session->socket, ": ", message);
}

template<class CoreModule>
void Session<CoreModule>::UpdateLastActivityTime() {
    last_activity_time = time(nullptr);
}