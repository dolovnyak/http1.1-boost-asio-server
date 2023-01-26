#pragma once

#include "MiniEvent.h"

class MiniSession;

class DefaultCloseEvent : public MiniEvent {
public:
    DefaultCloseEvent(MiniSession* session);

    void Process() OVERRIDE;

private:
    MiniSession* _session;
};
