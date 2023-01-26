#pragma once

#ifdef _STANDARD98
#define OVERRIDE
#else
#define OVERRIDE override
#endif

enum InternalEventType {
    READ,
    WRITE,
    CLOSE
};
