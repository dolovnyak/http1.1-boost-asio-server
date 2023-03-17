#include "RequestHandler.h"
#include "Exception.h"
#include "CgiHandler.h"
#include "AutoindexHandler.h"

namespace Http {

namespace {

std::string ProcessHeadGet(const std::shared_ptr<Location>& matched_location, const std::shared_ptr<Request>& request,
                           const std::string& path_after_matching, bool should_read) {
    std::string result;
    std::string path = UnitePaths(matched_location->root, path_after_matching);

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
            return AutoindexHandler::Handle(matched_location, path, path_after_matching, request);
        }
    }

    throw NotFound("File not found", request->server_config);
}

}

bool RequestHandler::FindLocation(const std::string& path, size_t path_compare_len) {
    for (const auto& current_location: _request->server_config->locations) {
        if (path.compare(0, path_compare_len, current_location->location) == 0) {
            _matched_location = current_location;
            _path_after_matching = path.substr(path_compare_len, path.size() - path_compare_len);
            return true;
        }
    }
    return false;
}

void RequestHandler::HandleRouteLocation() {
    std::string path = _request->target.path;
    size_t path_len = path.size();

    for (auto& interceptor: _request->server_config->extensions_interceptors) {
        if (interceptor->extension == _request->target.extension &&
            interceptor->on_methods.find(_request->http_method) != interceptor->on_methods.end()) {
            _matched_interceptor = interceptor;
            return;
        }
    }

    while (true) {
        if (FindLocation(path, path_len)) {
            return;
        }
        path_len = path.rfind('/', path_len - 1);
        if (path_len == 0) {
            if (FindLocation(path, 1)) {
                return;
            }
        }
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
    /// keep-alive change session
    _keep_alive_timeout = _request->server_config->default_keep_alive_timeout_s;  /// default value

    auto it = _request->http_headers.find(KEEP_ALIVE);
    if (it == _request->http_headers.end()) {
        return;
    }

    const std::string& value = it->second.back();

    std::vector<std::string> tokens = SplitString(value, ",");

    for (const auto& token: tokens) {
        std::string parameter = StripString(token);
        std::vector<std::string> parameter_tokens = SplitString(parameter, "=");

        if (parameter_tokens.size() == 2) {
            if (ToLower(parameter_tokens[0]) == TIMEOUT && IsPositiveNumberString(parameter_tokens[1])) {
                _keep_alive_timeout = std::min(_request->server_config->max_keep_alive_timeout_s,
                                               static_cast<unsigned int>(ParsePositiveInt(parameter_tokens[1])));
            }
        }
        /// MAX parameter is outdated and not supported
    }
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
        case Http::Method::Put:
        default:
            break;
    }

    if (_matched_interceptor.has_value()) {
        return;
    }
    if (_request->content_length.value_or(0) > _matched_location->max_body_size) {
        throw PayloadTooLarge("body larger than max_body_size", _request->server_config);
    }
}

void RequestHandler::HandleCookiesHeader() {
    /// TODO cookies
}

void RequestHandler::HandleAuthorizationHeader() {
    /// TODO authorization
}

std::shared_ptr<Response> RequestHandler::HandleHttpMethod() {
    /// handle interceptors
    if (_matched_interceptor.has_value()) {
        std::string script_path = _matched_interceptor.value()->cgi_path;
        CgiHandler handler(_request, _content_type, script_path, {});
        return handler.Handle();
    }

    /// handle methods
    auto it = _matched_location->available_methods.find(_request->http_method);
    if (it == _matched_location->available_methods.end()) {
        throw MethodNotAllowed("", _request->server_config);
    }

    /// handle Http Return
    if (_matched_location->http_return.has_value()) {
        auto headers = Response::GetDefaultHeaders(_request->server_config);
        if (_matched_location->http_return->redirect.has_value()) {
            headers.emplace_back(LOCATION_KEY, _matched_location->http_return->redirect.value());
        }
        return Response::MakeDefaultWithBody(_request->server_config,
                                             _matched_location->http_return->code,
                                             ToString(_matched_location->http_return->code),
                                             "", _keep_alive);
    }

    switch (*it) {
        case Http::Method::Get: {
            std::string body = ProcessHeadGet(_matched_location, _request, _path_after_matching, true);
            return Response::MakeDefaultWithBody(_request->server_config, Code::Ok, ToString(Code::Ok), std::move(body),
                                                 _keep_alive);
        }

        case Http::Method::Head: {
            ProcessHeadGet(_matched_location, _request, _path_after_matching, false);
            return Response::MakeDefaultWithoutBody(_request->server_config, Code::Ok, ToString(Code::Ok), _keep_alive);
        }

        case Http::Method::Post: {
            std::string script_path = UnitePaths(_matched_location->root, _path_after_matching);
            CgiHandler handler(_request, _content_type, script_path, {});
            return handler.Handle();
        }

        case Http::Method::Put: {
            std::string upload_path = UnitePaths(_matched_location->root, _path_after_matching);
            EnvironmentVariable upload_path_var(CGI_KEY_PATH_TO_CATION, upload_path);
            std::string script_uploader_path = _request->server_config->cgi_uploader_path;
            CgiHandler handler(_request, _content_type, script_uploader_path, {upload_path_var});
            return handler.Handle();
        }

        case Http::Method::Delete: {
            std::string delete_path = UnitePaths(_matched_location->root, _path_after_matching);
            EnvironmentVariable delete_path_var(CGI_KEY_PATH_TO_CATION, delete_path);
            std::string script_deleter_path = _request->server_config->cgi_deleter_path;
            CgiHandler handler(_request, _content_type, script_deleter_path, {delete_path_var});
            return handler.Handle();
        }

        case Http::Method::Options:
        case Http::Method::Connect:
        case Http::Method::Trace:
        case Http::Method::Patch:
            throw NotImplemented("Method " + ToString(*it) + " not implemented",
                                 _request->server_config);
    }
    throw std::logic_error("case missing");
}

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
    return {_request, response, _keep_alive, _keep_alive_timeout};
}

RequestHandler::RequestHandler(
        const std::shared_ptr<Request>& request)
        : _request(request) {}

}
