#pragma once

#include "SharedPtr.h"
#include "HandleHttpRequestEvent.h"
#include "ProcessRequestEvent.h"
#include "HttpSession.h"
#include "FileSession.h"

template<class CoreModule>
class EventMaker {
public:
    static SharedPtr<Event> MakeReadEvent(SharedPtr<Session<CoreModule> > session,
                                          const SharedPtr<std::string>& incoming_data,
                                          std::queue<SharedPtr<Event> >* event_queue) {

        if (dynamic_cast<HttpSession<CoreModule>*>(session.Get()) != nullptr) {
            return MakeShared<Event>(new HandleHttpRequestEvent<CoreModule>(session, incoming_data, event_queue));
        }

        if (dynamic_cast<FileSession<CoreModule>*>(session.Get()) != nullptr) {
            // TODO
        }

        throw std::logic_error("Unknown session type");
    }
};