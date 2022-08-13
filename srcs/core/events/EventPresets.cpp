#include "EventPresets.h"
#include "HttpModule.h"

Event EventPresets::ParseHttpRequest(const std::shared_ptr<Connection>& connection,
                                     const std::shared_ptr<std::string>& raw_request,
                                     std::queue<Event>* event_queue) {

    auto parse_http = [connection, raw_request, event_queue](){
        Request request = HttpModule::ProcessParseHttpRequest(connection, raw_request);
        event_queue->push(EventPresets::MakeHttpResponse(connection, request));
    };

    return {EventType::ParseHttpRequest, parse_http};
}

//Event EventPresets::ParseHttpRequest(const std::shared_ptr<Connection>& connection,
//                                     const std::shared_ptr<std::string>& raw_request,
//                                     std::queue<Event>* event_queue) {
//
//    auto parse_http = [connection, raw_request, event_queue](){
//        event_queue->push(EventPresets::MakeHttpResponse(connection, fut));
//    };
//
//    return {EventType::ParseHttpRequest, parse_http};
//}

//Event EventPresets::WaitResult(const std::shared_ptr<Connection>& connection,
//                                     const std::shared_ptr<std::string>& raw_request,
//                                     std::queue<Event>* event_queue) {
//
//    auto parse_http = [connection, raw_request, std::future fut](){
//        if (fut.ready) {
//            event_queue->push(EventPresets::MakeHttpResponse(connection, fut));
//        }
//        else {
//            event_queue->push(EventPresets::WaitResult(connection, raw_request, event_queue));
//        }
////        event_queue->push(EventPresets::MakeHttpResponse(connection, request));
//    };
//
//    return {EventType::ParseHttpRequest, parse_http};
//}

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
