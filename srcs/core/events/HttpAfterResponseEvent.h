#pragma once

#include "Event.h"
#include "Session.h"
#include "SharedPtr.h"
#include "Http.h"
#include "utilities.h"
#include "HttpSession.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

template<class CoreModule>
class HttpAfterResponseEvent : public Event {
public:
    HttpAfterResponseEvent(const SharedPtr<Session<CoreModule>>& session, std::queue<SharedPtr<Event> >* event_queue)
            : _packaged_http_session(session),
              _event_queue(event_queue) {}

    ~HttpAfterResponseEvent() {};

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;

private:
    SharedPtr<Session<CoreModule> > _packaged_http_session;
    HttpSession<CoreModule>* _http_session;
    std::queue<SharedPtr<Event> >* _event_queue;
};

template<class CoreModule>
void HttpAfterResponseEvent<CoreModule>::Process() {
    if (!_packaged_http_session->available) {
        LOG_INFO(GetName(), " on closed connection");
        return;
    }

    _http_session = dynamic_cast<HttpSession<CoreModule>*>(_packaged_http_session.Get());
    if (_http_session == nullptr) {
        LOG_ERROR(GetName(), " on non-http session");
        return;
    }

    if (_http_session->state != ConnectionState::ProcessRequest) {
        LOG_INFO(GetName(), " on wrong connection state");
        return;
    }

    if (!_http_session->keep_alive) {
        _http_session->Close();
        return;
    }
    else {
        _http_session->Clear();
    }
}

template<class CoreModule>
const std::string& HttpAfterResponseEvent<CoreModule>::GetName() const {
    static std::string kName = "HttpAfterResponseEvent";
    return kName;
}
