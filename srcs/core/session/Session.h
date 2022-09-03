#pragma once

#include "Event.h"

#include <queue>

template<class CoreModule>
class Session {
public:
    Session(int core_module_index, CoreModule* core_module, int socket)
            : available(true),
              core_module_index(core_module_index),
              core_module(core_module),
              socket(socket) {}

    virtual ~Session() {}

    virtual bool ShouldCloseAfterResponse() const = 0;

    virtual const std::string& GetResponseData() const = 0;

    void Close();

    bool available;

public:
    int core_module_index;
    CoreModule* core_module;
    int socket;
};

template<class CoreModule>
void Session<CoreModule>::Close() {
    this->core_module->CloseSession(core_module_index);
}