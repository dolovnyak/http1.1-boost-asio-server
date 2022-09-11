#pragma once

#include "Event.h"
#include "HttpException.h"
#include "SharedPtr.h"
#include "utilities.h"
#include "HttpProcessRequestEvent.h"
#include "HttpSession.h"

#include <queue>

template<class CoreModule>
class HttpReadRequestEvent : public Event {
public:
    HttpReadRequestEvent(const SharedPtr<Session<CoreModule> >& session,
                         const SharedPtr<std::string>& incoming_data,
                         std::queue<SharedPtr<Event> >* event_queue)
            : _packaged_http_session(session),
              _incoming_data(incoming_data),
              _event_queue(event_queue) {}

    ~HttpReadRequestEvent() {};

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;

private:
    SharedPtr<Session<CoreModule> > _packaged_http_session;
    SharedPtr<std::string> _incoming_data;
    std::queue<SharedPtr<Event> >* _event_queue;
    HttpSession<CoreModule>* _http_session;
};

template<class CoreModule>
const std::string& HttpReadRequestEvent<CoreModule>::GetName() const {
    static std::string kName = "HttpReadRequestEvent";
    return kName;
}

template<class CoreModule>
void HttpReadRequestEvent<CoreModule>::Process() {
    if (!_packaged_http_session->available) {
        LOG_INFO(GetName(), " on closed connection");
        return;
    }

    _http_session = dynamic_cast<HttpSession<CoreModule>*>(_packaged_http_session.Get());
    if (_http_session == nullptr) {
        LOG_ERROR(GetName(), " on non-http _http_session");
        return;
    }

    if (_http_session->state != HttpSessionState::ReadRequest) {
        LOG_INFO(GetName() + " on wrong connection state");
        return;
    }

    try {
        RequestHandleStatus::Status status = _http_session->request->Handle(_incoming_data);

        switch (status) {
            case RequestHandleStatus::Finish:
                _http_session->state = HttpSessionState::ProcessRequest;
                _event_queue->push(MakeShared<Event>(
                        new HttpProcessRequestEvent<CoreModule>(_packaged_http_session, _event_queue)));

            case RequestHandleStatus::WaitMoreData:
                return;
        }
    }
    catch (const HttpException& e) {
        _http_session->SendDataToClient(e.GetErrorResponse()->response, e.ShouldKeepAlive());
        LOG_INFO("HttpException: ", e.what());
    }
    catch (const std::exception& e) {
        LOG_ERROR("Unexpected exception: ", e.what());
        _http_session->SendDataToClient(Response::MakeErrorResponse(Http::InternalServerError,
                                                                    "Internal server error",
                                                                    _http_session->port_servers_config).response,
                                        false);
    }
}
