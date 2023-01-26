#pragma once

#include "SharedPtr.h"
#include "HttpSessionReadEvent.h"
#include "HttpSessionProcessRequestEvent.h"
#include "HttpFileSessionReadEvent.h"
#include "HttpSessionReadZeroEvent.h"
#include "HttpFileSessionReadZeroEvent.h"
#include "HttpSessionAfterResponseEvent.h"
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
                return MakeShared<Event>(new HttpSessionReadEvent<CoreModule>(session, incoming_data, event_queue));
            case SessionType::File:
                return MakeShared<Event>(new HttpFileSessionReadEvent<CoreModule>(session, incoming_data, event_queue));
            case SessionType::Server:
                throw std::logic_error("EventMaker::MakeReadEvent() should not be called for ServerSession");
        }
    }

    static SharedPtr<Event> MakeReadZeroBytesEvent(SharedPtr<Session<CoreModule> > session,
                                                   std::queue<SharedPtr<Event> >* event_queue) {
        switch (session->GetType()) {
            case SessionType::Http:
                return MakeShared<Event>(new HttpSessionReadZeroEvent<CoreModule>(session, event_queue));
            case SessionType::File:
                return MakeShared<Event>(new HttpFileSessionReadZeroEvent<CoreModule>(session, event_queue));
            case SessionType::Server:
                throw std::logic_error("EventMaker::MakeReadZeroBytesEvent() should not be called for ServerSession");
        }
    }

    static SharedPtr<Event> MakeAfterWriteEvent(SharedPtr<Session<CoreModule> > session,
                                                std::queue<SharedPtr<Event> >* event_queue) {
        switch (session->GetType()) {
            case SessionType::Http:
                return MakeShared<Event>(new HttpSessionAfterResponseEvent<CoreModule>(session, event_queue));
            case SessionType::File:
                throw std::logic_error("EventMaker::MakeAfterWriteEvent() should not be called for FileSession");
            case SessionType::Server:
                throw std::logic_error("EventMaker::MakeAfterWriteEvent() should not be called for ServerSession");
        }
    }
};
