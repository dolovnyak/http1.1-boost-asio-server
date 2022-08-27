#pragma once

#include "Event.h"
#include "HttpException.h"
#include "SharedPtr.h"
#include "utilities.h"
#include "ProcessIncomingDataEvent.h"

#include <queue>

template<class CoreModule>
class HandleIncomingDataEvent : public Event {
public:
    HandleIncomingDataEvent(const SharedPtr<Connection<CoreModule> >& connection,
                            const SharedPtr<std::string>& incoming_data,
                            std::queue<SharedPtr<Event> >* event_queue)
            : _connection(connection),
              _incoming_data(incoming_data),
              _event_queue(event_queue) {}

    ~HandleIncomingDataEvent() {};

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;


private:
    SharedPtr<Connection<CoreModule> > _connection;
    SharedPtr<std::string> _incoming_data;
    std::queue<SharedPtr<Event> >* _event_queue;
};

template<class CoreModule>
const std::string& HandleIncomingDataEvent<CoreModule>::GetName() const {
    static std::string kName = "HandleIncomingDataEvent";
    return kName;
}

template<class CoreModule>
void HandleIncomingDataEvent<CoreModule>::Process() {
    try {
        if (!_connection->available) {
            LOG_INFO(GetName() + " on closed connection");
            return;
        }
        if (_connection->state != ConnectionState::HandleRequest) {
            LOG_INFO(GetName() + " on wrong connection state");
            return;
        }

        RequestHandleStatus::Status status = _connection->request->Handle(_incoming_data);
        switch (status) {
            case RequestHandleStatus::Finish:
                _connection->state = ConnectionState::ProcessRequest;
                _event_queue->push(MakeShared<Event>(new ProcessIncomingDataEvent<CoreModule>(_connection, _event_queue)));

            case RequestHandleStatus::WaitMoreData:
                return;
        }
    }
    catch (const HttpException& e) {
        _connection->state = ConnectionState::ResponseToClient;
        _connection->SendErrorDataToClient(e.GetErrorResponse());
        LOG_INFO("HttpException: ", e.what());
    }
    catch (const std::exception& e) {
        LOG_ERROR("Unexpected exception: ", e.what());
    }
}
