#include "HttpModule.h"
#include "HttpErrorCodePages.h"

ParseRequestResult HttpModule::ParseRequest(std::shared_ptr<std::string> raw_request_part, Request request) {
    LOG_INFO("Process ParseHttpRequest");

    request.headers.content_length = 0;
    request.headers.dick_length = 25;
    request.body = "Hello World";
    request.status = RequestStatus::Success;
    ParseRequestResult parse_result;
    parse_result.status = ParseRequestStatus::Finish;
    parse_result.request = std::move(request);
    return parse_result;
}

Response HttpModule::ProcessRequest(Request request) {
    return Response();
}

Response HttpModule::MakeErrorResponse(RequestStatus status) {
    Response response;
    ///TODO setup response headers

    switch (status) {
        case RequestStatus::Success:
            LOG_ERROR("RequestStatus::Success in MakeErrorResponse");
            break;
        case RequestStatus::BadRequest:
            response.body = HttpErrorCodePages::k400;
            break;
        case RequestStatus::NotFound:
            response.body = HttpErrorCodePages::k404;
            break;
        case RequestStatus::NotAllowed:
            response.body = HttpErrorCodePages::k405;
            break;
    }
    return response;
}

