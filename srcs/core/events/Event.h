#pragma once

#include "ServerInstance.h"
#include "Connection.h"
#include "logging.h"

enum class EventType {
    ParseHttpRequest = 0,
    ProcessRequest = 1,
};

class Event {
public:
    Event(EventType type, std::function<void()> process_function);

    void Process();

    const std::string& GetName() const;

private:
    EventType _type;
    std::function<void()> _process_function;
};
