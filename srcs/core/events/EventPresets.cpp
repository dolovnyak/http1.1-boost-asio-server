#include "EventPresets.h"
#include "HttpModule.h"

Event EventPresets::ParseHttpRequest(const std::shared_ptr<Connection>& connection,
                                     const std::shared_ptr<std::string>& raw_request,
                                     std::queue<Event>* event_queue) {

    auto parse_http = [connection, raw_request, event_queue](){
        Request request = HttpModule::ProcessParseHttpRequest(connection, raw_request);
        LOG_INFO("request was parsed, try to push in queue");
        event_queue->push(EventPresets::MakeHttpResponse(connection, request));
        LOG_INFO("push in queue successfully");
    };

    return {EventType::ParseHttpRequest, parse_http};
}

Event EventPresets::MakeHttpResponse(const std::shared_ptr<Connection>& connection, Request request) {
    auto make_response = [connection, request](){
        std::string response;
        if (request.body.empty()) {
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
        } else {
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello," + request.body + "!</h1></body></html>";
        }
        connection->SetResponse(response);
        connection->SetWriteEventToCoreModule();
    };

    return {EventType::MakeHttpResponse, make_response};
}
