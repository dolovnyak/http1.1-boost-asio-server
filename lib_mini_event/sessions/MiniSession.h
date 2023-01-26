#pragma once

#include "MiniEvent.h"

class LibMiniEvent;

class MiniSession {
public:
    virtual MiniEvent MakeReadMiniEvent() = 0;

    virtual MiniEvent MakeWriteMiniEvent() = 0;

    virtual MiniEvent MakeCloseMiniEvent();

protected:
    LibMiniEvent* _libMiniEvent;
};
