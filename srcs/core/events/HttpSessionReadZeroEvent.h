#pragma once

#include "Event.h"
#include "Session.h"
#include "std::shared_ptr.h"
#include "Http.h"
#include "utilities.h"
#include "HttpFileSession.h"
#include "HttpSession.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

template<class CoreModule>
class HttpSessionReadZeroEvent : public Event {
public:
    HttpSessionReadZeroEvent(const std::shared_ptr<Session<CoreModule>>& session, std::queue<std::shared_ptr<Event> >* event_queue)
            : _packaged_http_session(session),
              _event_queue(event_queue) {}

    ~HttpSessionReadZeroEvent() {};

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;

private:
    std::shared_ptr<Session<CoreModule> > _packaged_http_session;
    HttpSession<CoreModule>* _http_session;
    std::queue<std::shared_ptr<Event> >* _event_queue;
};

template<class CoreModule>
void HttpSessionReadZeroEvent<CoreModule>::Process() {
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
const std::string& HttpSessionReadZeroEvent<CoreModule>::GetName() const {
    static std::string kName = "HttpSessionReadZeroEvent";
    return kName;
}
