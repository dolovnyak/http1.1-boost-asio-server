#pragma once

#include "Event.h"
#include "Session.h"
#include "SharedPtr.h"
#include "Http.h"
#include "utilities.h"
#include "HttpFileSession.h"
#include "HttpSession.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

template<class CoreModule>
class HttpReadRequestZeroBytesEvent : public Event {
public:
    HttpReadRequestZeroBytesEvent(const SharedPtr<Session<CoreModule>>& session, std::queue<SharedPtr<Event> >* event_queue)
            : _packaged_http_session(session),
              _event_queue(event_queue) {}

    ~HttpReadRequestZeroBytesEvent() {};

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;

private:
    SharedPtr<Session<CoreModule> > _packaged_http_session;
    HttpSession<CoreModule>* _http_session;
    std::queue<SharedPtr<Event> >* _event_queue;
};

template<class CoreModule>
void HttpReadRequestZeroBytesEvent<CoreModule>::Process() {
    if (!_packaged_http_session->available) {
        LOG_INFO(GetName(), " on closed file");
        return;
    }

    _http_session = dynamic_cast<HttpSession<CoreModule>*>(_packaged_http_session.Get());
    if (_http_session == nullptr) {
        LOG_ERROR(GetName(), " on non-http _main_http_session");
        return;
    }

    LOG_INFO("Close http session on ", GetName(), _http_session->socket);
    _http_session->Close();
}

template<class CoreModule>
const std::string& HttpReadRequestZeroBytesEvent<CoreModule>::GetName() const {
    static std::string kName = "HttpReadRequestZeroBytesEvent";
    return kName;
}
