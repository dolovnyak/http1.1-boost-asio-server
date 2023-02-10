#include "RequestHandler.h"
#include "Exception.h"
#include "CgiHandler.h"

namespace Http {

void RequestHandler::HandleRouteLocation() {
    std::string path = _request->target.path;
    std::string prefix = _request->target.extension;
    size_t path_len = path.size();

    bool prefix_was_not_found_on_first_iteration = false;

    while (true) {
        for (const auto& current_location: _request->server_config->locations) {

            switch (current_location->GetType()) {
                case LocationType::Prefix: {
                    if (prefix_was_not_found_on_first_iteration) {
                        break;
                    }
                    else {
                        if (current_location->location == prefix) {
                            _matched_location = current_location;
                            return;
                        }
                    }
                    break;
                }

                case LocationType::Path: {
                    if (path.compare(0, path_len, current_location->location) == 0) {
                        _matched_location = current_location;
                        return;
                    }
                    break;
                }
            }
        }

        prefix_was_not_found_on_first_iteration = true;
        path_len = path.rfind('/', path_len - 1) + 1;
        if (path_len == std::string::npos || path_len == 0) {
            throw NotFound("Location not found", _request->server_config);
        }
    }
}

void RequestHandler::HandleConnectionHeader() {
    auto it = _request->http_headers.find(CONNECTION);

    if (it != _request->http_headers.end()) {
        if (ToLower(it->second.back()) == KEEP_ALIVE) {
            _keep_alive = true;
            return;
        }
        else if (ToLower(it->second.back()) == CLOSE) {
            _keep_alive = false;
            return;
        }
    }

    switch (_request->http_version) {
        case Http::Http1_0:
            _keep_alive = false;
            break;
        case Http::Http1_1:
            _keep_alive = true;
            break;
    }
}

void RequestHandler::HandleKeepAliveHeader() {
    /// TODO
    /// keep-alive change session
//    _keep_alive_timeout = _endpoint_config->default_keep_alive_timeout_s;  /// default value
//
//    auto it = headers.find(KEEP_ALIVE);
//    if (it == headers.end()) {
//        return;
//    }
//
//    const std::string& value = it->second.back();
//
//    std::vector<std::string> tokens = SplitString(value, ",");
//
//    for (const auto& token: tokens) {
//        std::string parameter = StripString(token);
//        std::vector<std::string> parameter_tokens = SplitString(parameter, "=");
//
//        if (parameter_tokens.size() == 2) {
//            if (ToLower(parameter_tokens[0]) == TIMEOUT && IsPositiveNumberString(parameter_tokens[1])) {
//                _keep_alive_timeout = std::min(_endpoint_config->max_keep_alive_timeout_s,
//                                              static_cast<unsigned int>(ParsePositiveInt(parameter_tokens[1])));
//            }
//        }
//        /// MAX parameter is outdated and not supported
//    }
}

void RequestHandler::HandleAcceptHeader() {
    /// Not using by server, may be used by cgi.
}

void RequestHandler::HandleAcceptEncodingHeader() {
    /// Not using by server, may be used by cgi.
}

void RequestHandler::HandleContentTypeHeader() {
    auto it = _request->http_headers.find(CONTENT_TYPE);
    if (it == _request->http_headers.end()) {
        _content_type = DEFAULT_CONTENT_TYPE;
        return;
    }

    for (auto& content_type : it->second) {
        _content_type += content_type + ", ";
    }
}

void RequestHandler::HandleContentLengthHeader() {
    /// No need to Handle, because content length is already parsed
}

void RequestHandler::HandleCookiesHeader() {
    /// TODO cookies
}

void RequestHandler::HandleAuthorizationHeader() {
    /// TODO authorization
}

void RequestHandler::HandleHttpMethod() {
    auto it = _matched_location->available_methods.find(_request->http_method);
    if (it == _matched_location->available_methods.end()) {
        throw MethodNotAllowed("", _request->server_config);
    }

    switch (*it) {
        case Http::Method::Get:
        case Http::Method::Head:
            break;

        case Http::Method::Post: {
            if (!_request->content_length.has_value()) {
                throw LengthRequired("Length required", _request->server_config);
            }
            break;
        }

        case Http::Method::Delete:
        case Http::Method::Put:
            break;

        case Http::Method::Options:
        case Http::Method::Connect:
        case Http::Method::Trace:
        case Http::Method::Patch:
            throw NotImplemented("Method " + ToString(*it) + " not implemented",
                                 _request->server_config);
    }
}


std::shared_ptr<Response> RequestHandler::ProcessRequest() {
    if (_matched_location->cgi_path.has_value()) {
        std::string body = CgiHandler::Handle(_request, _matched_location, _content_type);
        return Response::MakeSuccessResponse(Code::Ok, body, _request->server_config, {}, _keep_alive);
    }
    else if (_matched_location->http_return.has_value()) {
        switch (Http::GetCodeType(_matched_location->http_return->code)) {
            case CodeType::Informational:
            case CodeType::Success:
                return Response::MakeSuccessResponse(
                        _matched_location->http_return->code, "", _request->server_config, {}, _keep_alive);

            case CodeType::Redirection: {
                return Response::MakeRedirectResponse(_matched_location->http_return->code, _request->server_config,
                                              _matched_location->http_return->redirect, _keep_alive);
            }

            case CodeType::ClientError:
            case CodeType::ServerError:
                auto& exception = Http::Exception::GetByCode(_matched_location->http_return->code,
                                                             _request->server_config);
                return exception.GetErrorResponse();
        }
    }
    else {
        if (!_matched_location->root.has_value()) {
            throw NotFound("There is no root in location", _request->server_config);
        }
        std::string file_path = _matched_location->root.value() + _request->target.path;
        std::string result;

        /// try find and read file
        if (ReadFile(file_path, result)) {
            return Response::MakeSuccessResponse(Http::Code::Ok, result, _request->server_config, {}, _keep_alive);
        }

        /// if there is no file - try open index
        if (_matched_location->index.has_value()) {
            file_path = _matched_location->root.value() + _request->target.directory_path + _matched_location->index.value();
            if (ReadFile(file_path, result)) {
                return Response::MakeSuccessResponse(Http::Code::Ok, result, _request->server_config, {}, _keep_alive);
            }
        }

        /// if there is no index - but threre is autoindex return this

        /// else throw not found error
        throw NotFound("", _request->server_config);
    }
};

HandleResult RequestHandler::Handle() {
    /// never change order of these methods
    HandleRouteLocation();
    HandleContentTypeHeader();
    HandleContentLengthHeader();
    HandleConnectionHeader();
    HandleKeepAliveHeader();
    HandleAcceptHeader();
    HandleAcceptEncodingHeader();
    HandleCookiesHeader();
    HandleHttpMethod();

    return {_request, ProcessRequest()};
}

RequestHandler::RequestHandler(const std::shared_ptr<Request>& request)
        : _request(request) {}

}
