#include "RequestHandler.h"



void RequestHandler::HandleRouteLocation() {
    ///TODO
//    /// для _endpoint_config.
//    /// для каждого location проверить, что он подходит для данного пути
//    /// если ни для одного не подходит уменьшить путь до предыдущего слеша и проверить снова
//
//    std::string path = target.path;
//    size_t path_len = path.size();
//
//
//    while (true) {
//        for (const auto& current_location : _endpoint_config->locations) {
//            if (current_location->location.compare(0, path_len, path) == 0) {
//                location = current_location;
//                return;
//            }
//        }
//
//        path_len = path.rfind('/', path_len - 1);
//        if (path_len == 0 || path_len == std::string::npos) {
//            throw NotFound("Location not found", _endpoint_config);
//        }
//    }

//    _target.path = _endpoint_config->root + _target.path;
//    _target.directory_path = _endpoint_config->root + _target.directory_path;
//
//    if (_endpoint_config->cgi_file_extensions.find(_target.extension) !=
//        _endpoint_config->cgi_file_extensions.end()) {
//        is_cgi = true;
//    }
//    else {
//        if (_target.path == _target.directory_path) {
//            _target.path += _endpoint_config->default_file_name;
//        }
//        is_cgi = false;
//    }
}

void RequestHandler::HandleConnectionHeader() {
    /// TODO
//    auto it = headers.find(CONNECTION);
//
//    if (it != headers.end()) {
//        if (ToLower(it->second.back()) == KEEP_ALIVE) {
//            _keep_alive = true;
//        }
//        else if (ToLower(it->second.back()) == CLOSE) {
//            _keep_alive = false;
//        }
//        return;
//    }
//
//    switch (http_version) {
//        case Http::Http1_0:
//            _keep_alive = false;
//            break;
//        case Http::Http1_1:
//            _keep_alive = true;
//            break;
//    }
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
    /// Not using by server, may be used by cgi. By default response return "Content-Type: text/html"
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
//    switch (_request->http_method) {
//        case Http::Method::Get:
//        case Http::Method::Head:
//            break;
//
//
//        case Http::Method::Post: {
//            if (!_request->content_length.has_value()) {
//                throw LengthRequired("Length required", _request->_server_config);
//            }
//            if (!is_cgi) {
//                throw MethodNotAllowed("Method not allowed", _endpoint_config);
//            }
//            break;
//        }
//
//        case Http::Method::Delete: {
//            if (!is_cgi) {
//                throw MethodNotAllowed("Method not allowed", _endpoint_config);
//            }
//        }
//
//        case Http::Method::Put:
//        case Http::Method::Options:
//        case Http::Method::Connect:
//        case Http::Method::Trace:
//        case Http::Method::Patch:
//            throw NotImplemented("Method " + raw_method + " is not implemented", _endpoint_config);
//
//        case Http::Method::Unknown:
//            throw MethodNotAllowed("Method not allowed", _endpoint_config);
//    }
}


void RequestHandler::Handle() {
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
}

RequestHandler::RequestHandler(const std::shared_ptr<Request>& request) : _request(request){ }
