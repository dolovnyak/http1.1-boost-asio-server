#pragma once

#include "Http.h"
#include "HttpErrorPages.h"
#include "ServerInfo.h"
#include "SharedPtr.h"

/// У респонса должен быть набор базовых хедеров:
/// Date
/// Server
/// Content-Type
/// Content-Length
/// Connection: (keep-alive или close)

/// При error response все есть внутри кроме имени сервера, его надо пробрасывать.

/// у респонса будет конструктор, который будет принимать статус ответа, тайтл, вектор хедеров и тело.

class Response {
public:
    static Response MakeErrorResponse(Http::Code code, const std::string& error_title,
                                      SharedPtr<ServerInfo> server_instance_info);

    Response(Http::Code code, const std::string& title,
             const std::vector<Http::Header>& custom_headers,
             const std::string& body);

    std::string raw_response;

private:

    std::string _body;
};
