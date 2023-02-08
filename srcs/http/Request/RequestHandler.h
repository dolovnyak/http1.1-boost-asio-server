#pragma once

#include "Request.h"

class RequestHandler {
private:
    std::shared_ptr<Request> _request;

public:
    RequestHandler(const std::shared_ptr<Request>& request);

    void Handle();

private:
    void HandleContentTypeHeader();

    void HandleContentLengthHeader();

    void HandleConnectionHeader();

    void HandleKeepAliveHeader();

    void HandleAcceptHeader();

    void HandleAcceptEncodingHeader();

    void HandleCookiesHeader();

    void HandleRouteLocation();

    void HandleAuthorizationHeader();

    void HandleHttpMethod();
};
