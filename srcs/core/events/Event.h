#pragma once

#include "HttpSession.h"
#include "Logging.h"

class Event {
public:
    virtual ~Event() {};

    virtual void Process() = 0;

    virtual const std::string& GetName() const = 0;
};
