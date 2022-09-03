#pragma once

#include "Event.h"
#include "Session.h"
#include "SharedPtr.h"
#include "Http.h"
#include "utilities.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

template<class CoreModule>
class HttpReadFileEvent : public Event {
public:
    HttpReadFileEvent(const SharedPtr<Session<CoreModule>>& session, const SharedPtr<std::string>& incoming_data, std::queue<SharedPtr<Event> >* event_queue)
            : _packaged_file_session(session),
              _incoming_data(incoming_data),
              _event_queue(event_queue) {}

    ~HttpReadFileEvent() {};

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;

private:
    SharedPtr<Session<CoreModule> > _packaged_file_session;
    HttpFileSession<CoreModule>* _file_session;
    SharedPtr<std::string> _incoming_data;
    std::queue<SharedPtr<Event> >* _event_queue;
};

template<class CoreModule>
void HttpReadFileEvent<CoreModule>::Process() {
    if (!_packaged_file_session->available) {
        LOG_INFO(GetName(), " on closed file");
        return;
    }

    _file_session = dynamic_cast<HttpFileSession<CoreModule>*>(_packaged_file_session.Get());
    if (_file_session == nullptr) {
        LOG_ERROR(GetName(), " on non-file _file_session");
        return;
    }

    if (!_file_session->main_http_session->available) {
        LOG_INFO(GetName(), " on closed main_http_session");
        _file_session->Close();
        return;
    }

    _file_session->read_data += *_incoming_data;
}

template<class CoreModule>
const std::string& HttpReadFileEvent<CoreModule>::GetName() const {
    static std::string kName = "HttpReadFileEvent";
    return kName;
}
