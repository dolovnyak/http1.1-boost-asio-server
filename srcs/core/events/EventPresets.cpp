#include "EventPresets.h"
#include "HttpModule.h"

Event EventPresets::ParseHttpRequest(const std::shared_ptr<Connection>& connection,
                                     const std::shared_ptr<std::string>& raw_request_part,
                                     std::queue<Event>* event_queue) {

    auto parse_http = [connection, raw_request_part, event_queue]() {
        if (!connection->still_available) {
            return;
        }

        ParseRequestResult result = HttpModule::ParseRequest(raw_request_part, std::move(connection->request));
        connection->request = std::move(result.request);

        if (result.status == ParseRequestStatus::Finish) {
            event_queue->push(EventPresets::ProcessHttpRequest(connection, event_queue));
        }
    };

    return {EventType::ParseHttpRequest, parse_http};
}

Event EventPresets::ProcessHttpRequest(const std::shared_ptr<Connection>& connection,
                                       std::queue<Event>* event_queue) {

    auto process_request = [connection, event_queue]() {
        if (!connection->still_available) {
            return;
        }

        if (connection->request.status == RequestStatus::Success) {
            connection->response = HttpModule::ProcessRequest(std::move(connection->request));
        }
        else {
            connection->response = HttpModule::MakeErrorResponse(connection->request.status);
        }
    };

    return {EventType::ProcessRequest, process_request};
}

//Event EventPresets::MakeHttpResponse(const std::shared_ptr<Connection>& connection, Request request) {
//    auto make_response = [connection, request](){
//        std::string response = request.body;
//        connection->SetResponse(response);
//        connection->SetWriteEventToCoreModule();
//    };
//
//    return {EventType::MakeHttpResponse, make_response};
//}

/// EventFSM
/// InitState -> ParseHttpRequest
/// ParseHttpRequest -> ParseHttpRequestWaitResult
/// ParseHttpRequestWaitResult -> ParseHttpRequestWaitResult
/// ParseHttpRequestWaitResult -> MakeHttpResponse
/// MakeHttpResponse -> MakeHttpResponseWaitResult
/// MakeHttpResponseWaitResult -> MakeHttpResponseWaitResult
/// MakeHttpResponseWaitResult -> WriteToSocket
