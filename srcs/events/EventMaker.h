#pragma once

#include "std::shared_ptr.h"
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
    static std::shared_ptr<Event> MakeReadEvent(std::shared_ptr<Session > session,
                                          const std::shared_ptr<std::string>& incoming_data,
                                          std::queue<std::shared_ptr<Event> >* event_queue) {
        switch (session->GetType()) {
            case SessionType::Http:
                return MakeShared<Event>(new HttpSessionReadEvent<CoreModule>(session, incoming_data, event_queue));
            case SessionType::File:
                return MakeShared<Event>(new HttpFileSessionReadEvent<CoreModule>(session, incoming_data, event_queue));
            case SessionType::Server:
                throw std::logic_error("EventMaker::MakeReadEvent() should not be called for ServerSession");
        }
    }

    static std::shared_ptr<Event> MakeReadZeroBytesEvent(std::shared_ptr<Session > session,
                                                   std::queue<std::shared_ptr<Event> >* event_queue) {
        switch (session->GetType()) {
            case SessionType::Http:
                return MakeShared<Event>(new HttpSessionReadZeroEvent<CoreModule>(session, event_queue));
            case SessionType::File:
                return MakeShared<Event>(new HttpFileSessionReadZeroEvent<CoreModule>(session, event_queue));
            case SessionType::Server:
                throw std::logic_error("EventMaker::MakeReadZeroBytesEvent() should not be called for ServerSession");
        }
    }

    static std::shared_ptr<Event> MakeAfterWriteEvent(std::shared_ptr<Session > session,
                                                std::queue<std::shared_ptr<Event> >* event_queue) {
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
