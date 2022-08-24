#pragma once

#include "ServerInstance.h"
#include "HttpConnection.h"
#include "Logging.h"

class Event {
public:
    virtual ~Event() {};

    virtual void Process() = 0;

    virtual const std::string& GetName() const = 0;
};

/// EventFSM
/// InitState -> ParseHttpRequest
/// ParseHttpRequest -> ParseHttpRequestWaitResult
/// ParseHttpRequestWaitResult -> ParseHttpRequestWaitResult
/// ParseHttpRequestWaitResult -> MakeHttpResponse
/// MakeHttpResponse -> MakeHttpResponseWaitResult
/// MakeHttpResponseWaitResult -> MakeHttpResponseWaitResult
/// MakeHttpResponseWaitResult -> WriteToSocket
