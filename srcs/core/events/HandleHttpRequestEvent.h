#pragma once

#include "Event.h"
#include "HttpException.h"
#include "SharedPtr.h"
#include "utilities.h"
#include "ProcessRequestEvent.h"
#include "HttpSession.h"

#include <queue>

template<class CoreModule>
class HandleHttpRequestEvent : public Event {
public:
    HandleHttpRequestEvent(const SharedPtr<Session<CoreModule> >& session,
                           const SharedPtr<std::string>& incoming_data,
                           std::queue<SharedPtr<Event> >* event_queue)
            : _session(session),
              _incoming_data(incoming_data),
              _event_queue(event_queue) {}

    ~HandleHttpRequestEvent() {};

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;

private:
    SharedPtr<Session<CoreModule> > _session;
    SharedPtr<std::string> _incoming_data;
    std::queue<SharedPtr<Event> >* _event_queue;
    HttpSession<CoreModule>* _http_session;
};

template<class CoreModule>
const std::string& HandleHttpRequestEvent<CoreModule>::GetName() const {
    static std::string kName = "HandleHttpRequestEvent";
    return kName;
}

template<class CoreModule>
void HandleHttpRequestEvent<CoreModule>::Process() {
    if (!_session->available) {
        LOG_INFO(GetName(), " on closed connection");
        return;
    }

    _http_session = dynamic_cast<HttpSession<CoreModule>*>(_session.Get());
    if (_http_session == nullptr) {
        LOG_ERROR(GetName(), " on non-http _http_session");
        return;
    }

    try {
        if (_http_session->state != ConnectionState::HandleRequest) {
            LOG_INFO(GetName() + " on wrong connection state");
            return;
        }

        RequestHandleStatus::Status status = _http_session->request->Handle(_incoming_data);

        switch (status) {
            case RequestHandleStatus::Finish:
                _http_session->state = ConnectionState::ProcessRequest;
                _event_queue->push(MakeShared<Event>(new ProcessRequestEvent<CoreModule>(_session, _event_queue)));

            case RequestHandleStatus::WaitMoreData:
                return;
        }
    }
    catch (const HttpException& e) {
        _http_session->SendDataToClient(e.GetErrorResponse(), e.ShouldKeepAlive());
        LOG_INFO("HttpException: ", e.what());
    }
    catch (const std::exception& e) {
        LOG_ERROR("Unexpected exception: ", e.what());
        _http_session->SendDataToClient(
                MakeShared(Response::MakeErrorResponse(Http::InternalServerError,
                                                       "Internal server error",
                                                       _http_session->server_config)),
                false);
    }
}
