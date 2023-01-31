#pragma once

#include "Event.h"
#include "HttpException.h"
#include "std::shared_ptr.h"
#include "utilities.h"
#include "HttpSessionProcessRequestEvent.h"
#include "HttpSession.h"

#include <queue>

template<class CoreModule>
class HttpSessionReadEvent : public Event {
public:
    HttpSessionReadEvent(const std::shared_ptr<Session >& session,
                         const std::shared_ptr<std::string>& incoming_data,
                         std::queue<std::shared_ptr<Event> >* event_queue)
            : _packaged_http_session(session),
              _incoming_data(incoming_data),
              _event_queue(event_queue) {}

    ~HttpSessionReadEvent() {};

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;

private:
    std::shared_ptr<Session > _packaged_http_session;
    std::shared_ptr<std::string> _incoming_data;
    std::queue<std::shared_ptr<Event> >* _event_queue;
    HttpSession* _http_session;
};

template<class CoreModule>
const std::string& HttpSessionReadEvent<CoreModule>::GetName() const {
    static std::string kName = "HttpSessionReadEvent";
    return kName;
}

template<class CoreModule>
void HttpSessionReadEvent<CoreModule>::Process() {
    if (!_packaged_http_session->available) {
        LOG_INFO(GetName(), " on closed connection");
        return;
    }

    _http_session = dynamic_cast<HttpSession*>(_packaged_http_session.Get());
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
                        new HttpSessionProcessRequestEvent<CoreModule>(_packaged_http_session, _event_queue)));

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
