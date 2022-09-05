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
class HttpReadFileZeroBytesEvent : public Event {
public:
    HttpReadFileZeroBytesEvent(const SharedPtr<Session<CoreModule>>& session,
                               std::queue<SharedPtr<Event> >* event_queue)
            : _packaged_file_session(session),
              _event_queue(event_queue) {}

    ~HttpReadFileZeroBytesEvent() {};

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;

private:
    SharedPtr<Session<CoreModule> > _packaged_file_session;
    HttpFileSession<CoreModule>* _file_session;
    HttpSession<CoreModule>* _main_http_session;
    std::queue<SharedPtr<Event> >* _event_queue;
};

template<class CoreModule>
void HttpReadFileZeroBytesEvent<CoreModule>::Process() {
    if (!_packaged_file_session->available) {
        LOG_INFO(GetName(), " on closed file");
        return;
    }

    _file_session = dynamic_cast<HttpFileSession<CoreModule>*>(_packaged_file_session.Get());
    if (_file_session == nullptr) {
        LOG_ERROR(GetName(), " on non-file _file_session");
        return;
    }

    _main_http_session = dynamic_cast<HttpSession<CoreModule>*>(_file_session->main_http_session.Get());
    if (_main_http_session == nullptr) {
        LOG_ERROR(GetName(), " on non-http _main_http_session");
        return;
    }

    if (!_main_http_session->available) {
        LOG_INFO(GetName(), " on closed http");
        _file_session->Close();
        return;
    }

    if (_main_http_session->state != HttpSessionState::ProcessResource) {
        LOG_INFO(GetName(), " on wrong connection state");
        return;
    }

    _main_http_session->SendDataToClient(
            Response::MakeOkResponse(_file_session->read_data,
                                     _main_http_session->server_config,
                                     _main_http_session->keep_alive).response,
            _main_http_session->keep_alive);
    _file_session->Close();
}

template<class CoreModule>
const std::string& HttpReadFileZeroBytesEvent<CoreModule>::GetName() const {
    static std::string kName = "HttpReadFileZeroBytesEvent";
    return kName;
}
