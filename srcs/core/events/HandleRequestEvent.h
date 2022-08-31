#pragma once

#include "Event.h"
#include "HttpException.h"
#include "SharedPtr.h"
#include "utilities.h"
#include "ProcessRequestEvent.h"

#include <queue>

template<class CoreModule>
class HandleRequestEvent : public Event {
public:
    HandleRequestEvent(const SharedPtr<Session<CoreModule> >& connection,
                       const SharedPtr<std::string>& incoming_data,
                       std::queue<SharedPtr<Event> >* event_queue)
            : _session(connection),
              _incoming_data(incoming_data),
              _event_queue(event_queue) {}

    ~HandleRequestEvent() {};

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;


private:
    SharedPtr<Session<CoreModule> > _session;
    SharedPtr<std::string> _incoming_data;
    std::queue<SharedPtr<Event> >* _event_queue;
};

template<class CoreModule>
const std::string& HandleRequestEvent<CoreModule>::GetName() const {
    static std::string kName = "HandleRequestEvent";
    return kName;
}

template<class CoreModule>
void HandleRequestEvent<CoreModule>::Process() {
    try {
        if (!_session->available) {
            LOG_INFO(GetName() + " on closed connection");
            return;
        }
        if (_session->state != ConnectionState::HandleRequest) {
            LOG_INFO(GetName() + " on wrong connection state");
            return;
        }

        RequestHandleStatus::Status status = _session->request->Handle(_incoming_data);

        switch (status) {
            case RequestHandleStatus::Finish:
                _session->state = ConnectionState::ProcessRequest;
                _event_queue->push(MakeShared<Event>(new ProcessRequestEvent<CoreModule>(_session, _event_queue)));

            case RequestHandleStatus::WaitMoreData:
                return;
        }
    }
    catch (const HttpException& e) {
        _session->state = ConnectionState::ResponseToClient;
        _session->SendErrorDataToClient(e.GetErrorResponse());
        LOG_INFO("HttpException: ", e.what());
    }
    catch (const std::exception& e) {
        LOG_ERROR("Unexpected exception: ", e.what());
    }
}
