#include "HandleHttpRequestEvent.h"
#include "HttpException.h"

HandleHttpRequestEvent::HandleHttpRequestEvent(const SharedPtr<Connection>& connection,
                                               const SharedPtr<std::string>& raw_request_part,
                                               std::queue<Event>* event_queue)
                                             : _connection(connection),
                                               _raw_request_part(raw_request_part),
                                               _event_queue(event_queue) {}

void HandleHttpRequestEvent::Process() {
    if (!_connection->still_available) {
        return;
    }

    try {
        if (_connection->request->Handle(_raw_request_part) == RequestHandleStatus::Finish) {
//            _event_queue->push(ResponseEvent(_connection, _connection->response));
        }
    }
    catch (const HttpException& e) {
//        _event_queue->push(ResponseEvent(_connection, e.response));
    }

}

HandleHttpRequestEvent::~HandleHttpRequestEvent() {

}
