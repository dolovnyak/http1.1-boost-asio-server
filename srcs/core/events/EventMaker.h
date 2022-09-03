#pragma once

#include "SharedPtr.h"
#include "HttpReadRequestEvent.h"
#include "HttpProcessRequestEvent.h"
#include "HttpReadFileEvent.h"
#include "HttpReadRequestZeroBytesEvent.h"
#include "HttpReadFileZeroBytesEvent.h"
#include "HttpAfterResponseEvent.h"
#include "HttpSession.h"
#include "HttpFileSession.h"

template<class CoreModule>
class EventMaker {
public:
    static SharedPtr<Event> MakeReadEvent(SharedPtr<Session<CoreModule> > session,
                                          const SharedPtr<std::string>& incoming_data,
                                          std::queue<SharedPtr<Event> >* event_queue) {

        if (dynamic_cast<HttpSession<CoreModule>*>(session.Get()) != nullptr) {
            return MakeShared<Event>(new HttpReadRequestEvent<CoreModule>(session, incoming_data, event_queue));
        }

        if (dynamic_cast<HttpFileSession<CoreModule>*>(session.Get()) != nullptr) {
            return MakeShared<Event>(new HttpReadFileEvent<CoreModule>(session, incoming_data, event_queue));
        }

        throw std::logic_error("Unknown session type in EventMaker::MakeReadEvent");
    }

    static SharedPtr<Event> MakeReadZeroBytesEvent(SharedPtr<Session<CoreModule> > session,
                                                   std::queue<SharedPtr<Event> >* event_queue) {

        if (dynamic_cast<HttpSession<CoreModule>*>(session.Get()) != nullptr) {
            return MakeShared<Event>(new HttpReadRequestZeroBytesEvent<CoreModule>(session, event_queue));
        }

        if (dynamic_cast<HttpFileSession<CoreModule>*>(session.Get()) != nullptr) {
            return MakeShared<Event>(new HttpReadFileZeroBytesEvent<CoreModule>(session, event_queue));
        }

        throw std::logic_error("Unknown session type in EventMaker::MakeReadZeroBytesEvent");
    }

    static SharedPtr<Event> MakeAfterWriteEvent(SharedPtr<Session<CoreModule> > session,
                                                std::queue<SharedPtr<Event> >* event_queue) {

        if (dynamic_cast<HttpSession<CoreModule>*>(session.Get()) != nullptr) {
            return MakeShared<Event>(new HttpAfterResponseEvent<CoreModule>(session, event_queue));
        }

        throw std::logic_error("MakeAfterWriteEvent: Only HttpSession could write to client");
    }
};
