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
        switch (session->GetType()) {
            case SessionType::Http:
                return MakeShared<Event>(new HttpReadRequestEvent<CoreModule>(session, incoming_data, event_queue));
            case SessionType::File:
                return MakeShared<Event>(new HttpReadFileEvent<CoreModule>(session, incoming_data, event_queue));
            case SessionType::Server:
                throw std::logic_error("EventMaker::MakeReadEvent() should not be called for ServerSession");
        }
    }

    static SharedPtr<Event> MakeReadZeroBytesEvent(SharedPtr<Session<CoreModule> > session,
                                                   std::queue<SharedPtr<Event> >* event_queue) {
        switch (session->GetType()) {
            case SessionType::Http:
                return MakeShared<Event>(new HttpReadRequestZeroBytesEvent<CoreModule>(session, event_queue));
            case SessionType::File:
                return MakeShared<Event>(new HttpReadFileZeroBytesEvent<CoreModule>(session, event_queue));
            case SessionType::Server:
                throw std::logic_error("EventMaker::MakeReadZeroBytesEvent() should not be called for ServerSession");
        }
    }

    static SharedPtr<Event> MakeAfterWriteEvent(SharedPtr<Session<CoreModule> > session,
                                                std::queue<SharedPtr<Event> >* event_queue) {
        switch (session->GetType()) {
            case SessionType::Http:
                return MakeShared<Event>(new HttpAfterResponseEvent<CoreModule>(session, event_queue));
            case SessionType::File:
                throw std::logic_error("EventMaker::MakeAfterWriteEvent() should not be called for FileSession");
            case SessionType::Server:
                throw std::logic_error("EventMaker::MakeAfterWriteEvent() should not be called for ServerSession");
        }
    }
};
