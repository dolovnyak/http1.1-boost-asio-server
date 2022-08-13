#include "EventPresets.h"
#include "HttpModule.h"

Event EventPresets::ParseHttpRequest(const std::shared_ptr<Connection>& connection,
                                     const std::shared_ptr<std::string>& raw_request,
                                     std::queue<Event>* event_queue) {

    auto parse_http = [connection, raw_request, event_queue](){
        if (connection->IsAvailable()) {

//            std::pair<Request, bool> request_with_status = HttpModule::ProcessParseHttpRequest(connection, raw_request);
//            if (request_with_status.second) {
//                event_queue->push(Event::MakeHttpResponse(connection, request_with_status.first));
//            }
        }

    };

    return {EventType::ParseHttpRequest, parse_http};
}

Event EventPresets::MakeHttpResponse(const std::shared_ptr<Connection>& connection, Request request) {
    auto make_response = [connection, request](){
        std::string response = request.body;
        connection->SetResponse(response);
        connection->SetWriteEventToCoreModule();
    };

    return {EventType::MakeHttpResponse, make_response};
}

/// EventFSM
/// InitState -> ParseHttpRequest
/// ParseHttpRequest -> ParseHttpRequestWaitResult
/// ParseHttpRequestWaitResult -> ParseHttpRequestWaitResult
/// ParseHttpRequestWaitResult -> MakeHttpResponse
/// MakeHttpResponse -> MakeHttpResponseWaitResult
/// MakeHttpResponseWaitResult -> MakeHttpResponseWaitResult
/// MakeHttpResponseWaitResult -> WriteToSocket
