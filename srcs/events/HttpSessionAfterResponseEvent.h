#pragma once

#include "Event.h"
#include "Session.h"
#include "std::shared_ptr.h"
#include "Http.h"
#include "utilities.h"
#include "HttpSession.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

template<class CoreModule>
class HttpSessionAfterResponseEvent : public Event {
public:
    HttpSessionAfterResponseEvent(const std::shared_ptr<Session>& session, std::queue<std::shared_ptr<Event> >* event_queue)
            : _packaged_http_session(session),
              _event_queue(event_queue) {}

    ~HttpSessionAfterResponseEvent() {};

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;

private:
    std::shared_ptr<Session > _packaged_http_session;
    HttpSession* _http_session;
    std::queue<std::shared_ptr<Event> >* _event_queue;
};

template<class CoreModule>
void HttpSessionAfterResponseEvent<CoreModule>::Process() {
    if (!_packaged_http_session->available) {
        LOG_INFO(GetName(), " on closed connection");
        return;
    }

    _http_session = dynamic_cast<HttpSession*>(_packaged_http_session.Get());
    if (_http_session == nullptr) {
        LOG_ERROR(GetName(), " on non-http session");
        return;
    }

    if (_http_session->state != HttpSessionState::ResponseToClient) {
        LOG_INFO(GetName(), " on wrong session state");
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
const std::string& HttpSessionAfterResponseEvent<CoreModule>::GetName() const {
    static std::string kName = "HttpSessionAfterResponseEvent";
    return kName;
}
