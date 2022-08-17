#include "ParseHttpRequestEvent.h"

ParseHttpRequestEvent::ParseHttpRequestEvent(const SharedPtr<Connection>& connection,
                                             const SharedPtr<std::string>& raw_request_part,
                                             std::queue<Event>* event_queue)
                                             : _connection(connection),
                                               _raw_request_part(raw_request_part),
                                               _event_queue(event_queue) {}

void ParseHttpRequestEvent::Process() {
    if (!_connection->still_available) {
        return;
    }

    RequestParseStatus status = _connection->request->Parse(_raw_request_part);

    switch (status) {
//        case RequestParseStatus::FinishWithSuccess:
//            _event_queue->push(EventPresets::MakeHttpResponse(_connection, *_connection->request));
//            break;
//        case RequestParseStatus::FinishWithError:
//            HttpModule::MakeErrorResponse();
        case RequestParseStatus::WaitMoreData:
            break;
    }
}

ParseHttpRequestEvent::~ParseHttpRequestEvent() {

}
