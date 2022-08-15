#pragma once

#include "ServerInstance.h"
#include "Connection.h"
#include "logging.h"

//enum EventType {
//    ParseHttpRequest = 0,
//    ProcessRequest = 1,
//};

class Event {
public:
    virtual ~Event() {}

    virtual void Process() = 0;

    virtual const std::string& GetName() const = 0;
};
