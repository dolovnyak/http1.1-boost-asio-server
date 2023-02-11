#include "RequestHandler.h"
#include "Exception.h"
#include "CgiHandler.h"

namespace Http {

namespace {

std::string ProcessHeadGet(const std::shared_ptr<Location>& matched_location, const std::shared_ptr<Request>& request,
                           const std::string& path_after_matching, bool should_read) {
    if (!matched_location->root.has_value()) {
        throw NotFound("There is no root in location", request->server_config);
    }

    std::string result;
    std::string path = UnitePaths(matched_location->root.value(), path_after_matching);

    if (IsFile(path)) {
        if (!should_read) {
            return {};
        }
        if (ReadFile(path, result)) {
            return result;
        }
    }
    else if (IsDirectory(path)) {
        if (matched_location->index.has_value()) {
            std::string index_file_path = UnitePaths(path, matched_location->index.value());
            if (IsFile(index_file_path)) {
                if (!should_read) {
                    return {};
                }
                if (ReadFile(index_file_path, result)) {
                    return result;
                }
            }
        }

        if (matched_location->autoindex) {
            if (!should_read) {
                return {};
            }
            /// Process autoindex
        }
    }

    throw NotFound("File not found", request->server_config);
}

std::shared_ptr<Response> ProcessPost(const std::shared_ptr<Location>& matched_location,
                                      const std::shared_ptr<Request>& request, const std::string& path_after_matching,
                                      const std::string& content_type) {

    std::string path = UnitePaths(matched_location->root.value(), path_after_matching);
    if (IsExecutableFile(path)) {
        std::string result = CgiHandler::Handle(request, matched_location, content_type, path, {});
        return Response::MakeSuccessGetResponse(result, request->server_config, true);
    }
    throw NotFound("Script not found", request->server_config);
}

std::shared_ptr<Response> ProcessPut(const std::shared_ptr<Location>& matched_location,
                                     const std::shared_ptr<Request>& request,
                                     const std::string& content_type) {
    if (matched_location->upload_path.has_value()) {
        throw NotAcceptable("There is no path to upload", request->server_config);
    }

    if (IsExecutableFile(request->server_config->cgi_uploader_path)) {
        std::string result = CgiHandler::Handle(
                request, matched_location, content_type, request->server_config->cgi_uploader_path,
                {EnvironmentVariable(PATH_TO_SAVE_CGI_KEY, matched_location->upload_path.value())});
        return Response::MakeSuccessGetResponse(result, request->server_config, true);
    }
    throw InternalServerError("Uploader script not executable", request->server_config);
}

}

void RequestHandler::HandleRouteLocation() {
    std::string path = _request->target.path;
    size_t path_len = path.size();

    while (true) {
        for (const auto& current_location: _request->server_config->locations) {
            if (path.compare(0, path_len, current_location->location) == 0) {
                _matched_location = current_location;
                _path_after_matching = path.substr(path_len, path.size() - path_len);
                return;
            }
        }

        path_len = path.rfind('/', path_len - 1);
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
    /// TODO del or parse and transfer to CGI
}

void RequestHandler::HandleAcceptEncodingHeader() {
    /// TODO del or parse and transfer to CGI
}

void RequestHandler::HandleContentTypeHeader() {
    auto it = _request->http_headers.find(CONTENT_TYPE);
    if (it == _request->http_headers.end()) {
        _content_type = DEFAULT_CONTENT_TYPE;
        return;
    }

    _content_type = it->second.front();
}

void RequestHandler::HandleContentLengthHeader() {
    switch (_request->http_method) {
        case Http::Method::Post: {
            if (!_request->content_length.has_value()) {
                throw LengthRequired("Length required", _request->server_config);
            }
            break;
        }

        case Http::Method::Delete:
        case Http::Method::Put: /// TODO maybe it's need length too
        default:
            break;
    }

    /// TODO check max body
//    if (_request->content_length.value_or(0) > _matched_location)
}

void RequestHandler::HandleCookiesHeader() {
    /// TODO cookies
}

void RequestHandler::HandleAuthorizationHeader() {
    /// TODO authorization
}

std::shared_ptr<Response> RequestHandler::HandleHttpMethod() {
    /// check ExtensionInterceptors if there is no then process methods
    /// если расширение этого файла есть в ExtensionInterceptor - то даже если файла не существует вызвать обработчик cgi-path этого перехватывателя

    auto it = _matched_location->available_methods.find(_request->http_method);
    if (it == _matched_location->available_methods.end()) {
        throw MethodNotAllowed("", _request->server_config);
    }

    /// check if matched location has return code if it has then return it


    switch (*it) {
        case Http::Method::Get: {
            std::string body = ProcessHeadGet(_matched_location, _request, _path_after_matching, true);
            return Response::MakeSuccessGetResponse(body, _request->server_config, _keep_alive);
        }

        case Http::Method::Head: {
            ProcessHeadGet(_matched_location, _request, _path_after_matching, false);
            return Response::MakeSuccessHeadResponse(_request->server_config, _keep_alive);
        }

        case Http::Method::Post: {
            return ProcessPost(_matched_location, _request, _path_after_matching, _content_type);
        }

        case Http::Method::Delete: {
            /// если файл есть - удалить
        }

        case Http::Method::Put: {
            ProcessPut(_matched_location, _request, _content_type);
        }

        case Http::Method::Options:
        case Http::Method::Connect:
        case Http::Method::Trace:
        case Http::Method::Patch:
            throw NotImplemented("Method " + ToString(*it) + " not implemented",
                                 _request->server_config);
    }
}

//std::shared_ptr<Response> RequestHandler::HandleRequest() {
//    if (_matched_location->http_return.has_value()) {
//        switch (Http::GetCodeType(_matched_location->http_return->code)) {
//            case CodeType::Informational:
//            case CodeType::Success:
//                return Response::MakeSuccessResponse(
//                        _matched_location->http_return->code, "", _request->server_config, {}, _keep_alive);
//
//            case CodeType::Redirection: {
//                return Response::MakeRedirectResponse(_matched_location->http_return->code,
//                                                      _request->server_config,
//                                                      _matched_location->http_return->redirect,
//                                                      _keep_alive);
//            }
//
//            case CodeType::ClientError:
//            case CodeType::ServerError:
//                auto& exception = Http::Exception::GetByCode(_matched_location->http_return->code,
//                                                             _request->server_config);
//                return exception.GetErrorResponse();
//        }
//    }
//    else {
//    }
//}

HandleResult RequestHandler::Handle() {
    /// Never change order of these methods
    HandleRouteLocation();
    HandleContentLengthHeader();
    HandleContentTypeHeader();
    HandleConnectionHeader();
    HandleKeepAliveHeader();
    HandleAcceptHeader();
    HandleAcceptEncodingHeader();
    HandleCookiesHeader();
    HandleAuthorizationHeader();

    std::shared_ptr<Response> response = HandleHttpMethod();
    return {_request, response, _keep_alive};
}

RequestHandler::RequestHandler(
        const std::shared_ptr<Request>& request)
        : _request(request) {}

}
