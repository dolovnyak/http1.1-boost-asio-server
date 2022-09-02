#pragma once

#include "Event.h"

#include <queue>

template<class CoreModule>
class Session {
public:
    Session(int session_id, CoreModule* core_module)
            : available(true),
              _session_id(session_id),
              _core_module(core_module) {}

    virtual ~Session() {}

    virtual bool ShouldCloseAfterResponse() const = 0;

    bool available;

    std::string response_data;

protected:
    int _session_id;
    CoreModule* _core_module;
};
