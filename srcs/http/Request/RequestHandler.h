#pragma once

#include "Request.h"
#include "Response.h"

namespace Http {

struct HandleResult {
    std::shared_ptr<Request> handled_request;
    std::shared_ptr<Response> response;
    bool keep_alive;
};

class RequestHandler {
private:
    std::shared_ptr<Request> _request;

    std::optional<std::shared_ptr<ExtensionInterceptor>> _matched_interceptor;

    std::shared_ptr<Location> _matched_location;

    std::string _content_type;

    std::string _path_after_matching;

    bool _keep_alive;

public:
    RequestHandler(const std::shared_ptr<Request>& request);

    HandleResult Handle();

private:
    void HandleContentTypeHeader();

    void HandleContentLengthHeader();

    void HandleConnectionHeader();

    void HandleKeepAliveHeader();

    void HandleAcceptHeader();

    void HandleAcceptEncodingHeader();

    void HandleCookiesHeader();

    void HandleRouteLocation();

    std::shared_ptr<Response> HandleHttpMethod();

    void HandleAuthorizationHeader();
};

}