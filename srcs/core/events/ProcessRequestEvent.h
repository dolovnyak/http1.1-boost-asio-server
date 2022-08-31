#pragma once

#include "Event.h"
#include "HttpException.h"
#include "utilities.h"

#include <queue>
#include <sys/unistd.h>

template<class CoreModule>
class ProcessRequestEvent : public Event {
public:
    ProcessRequestEvent(const SharedPtr<Session<CoreModule> >& connection,
                        std::queue<SharedPtr<Event> >* event_queue)
            : _session(connection), _event_queue(event_queue) {}

    ~ProcessRequestEvent() {}

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;


private:
    void CgiProcess(const std::string& directory_path);
    void FileProcess(const std::string& directory_path);

    SharedPtr<Session<CoreModule> > _session;
    std::queue<SharedPtr<Event> >* _event_queue;
};


template<class CoreModule>
const std::string& ProcessRequestEvent<CoreModule>::GetName() const {
    static std::string kName = "ProcessRequestEvent";
    return kName;
}

//template<class CoreModule>
//void ProcessRequestEvent<CoreModule>::CgiProcess(const std::string& directory_path) {
//
//    if (_session->request->method == "GET") {
//        /// может запустить cgi процесс (не передавая ему тело)
//    }
//    else if (_session->request->method == "POST") {
//        /// может запустить cgi процесс (передавая ему тело)
//    }
//    else if (_session->request->GetMethod() == "DELETE") {
//        /// может запустить cgi процесс
//    }
//    else if (_session->request->GetMethod() == "HEAD") {
//        /// читает файл/запускает cgi процесс и возвращает только заголовки
//    }
//    else if (_session->request->GetMethod() == "PUT"
//             || _session->request->GetMethod() == "OPTIONS"
//             || _session->request->GetMethod() == "CONNECT"
//             || _session->request->GetMethod() == "TRACE"
//             || _session->request->GetMethod() == "PATCH") {
//        throw NotImplemented("Method " + _session->request->GetMethod() + " is not implemented",
//                             _session->server_instance);
//    }
//    else {
//        throw MethodNotAllowed("Method not allowed", _session->server_instance);
//    }
//}

//template<class CoreModule>
//void ProcessRequestEvent<CoreModule>::FileProcess(const std::string& directory_path) {
//    std::istream is(&_session->request->target.file_path);
//    if (!is) {
//        throw NotFound("File not found", _session->server_instance);
//    }
//
//}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::Process() {
    try {
        if (!_session->available) {
            LOG_INFO(GetName(), " on closed connection");
            return;
        }
        if (_session->state != ConnectionState::ProcessRequest) {
            LOG_INFO(GetName(), " on wrong connection state");
            return;
        }

//        /// git dir and check is it CGI or simple file
//        /// два класса FileHandler и CGIHandler
//        if (_session->request->target.file_path.rbegin() == '/') {
//            _session->request->target.file_path += _session->server_instance->default_file_name;
//        }
//        _session->request->target.file_path = _session->server_instance->root_path + _session->request->target.file_path;
//        std::string directory_path = _session->request->target.file_path.substr(0, _session->request->target.file_path.find_last_of('/'));
//
//        if (_session->server_instance->cgi_directory_paths.find(directory_path) != _session->server_instance->cgi_directory_paths.end()) {
//            /// file is cgi script
//            if (!access(_session->request->target.file_path, X_OK)) {
//                throw NotFound("File not found or not available", _session->server_instance);
//            }
//            CgiProcess(directory_path);
//        }
//        else {
//            /// file is simple file
//            if (!access(_session->request->target.file_path, R_OK)) {
//                throw NotFound("File not found or not available", _session->server_instance);
//            }
//            FileProcess(directory_path);
//        }
//
//
//        if (_session->request->method == "GET") {
//            /// может просто вернуть файл
//
//        }
//        else if (_session->request->method == "POST") {
//            if (!_session->request->content_length.HasValue()) {
//                throw LengthRequired("Length required", _session->server_instance);
//            }
//        }
//        else if (_session->request->GetMethod() == "DELETE") {
//            /// может запустить cgi процесс
//            /// может удалить файл (если есть права)
//        }
//        else if (_session->request->GetMethod() == "HEAD") {
//            /// читает файл/запускает cgi процесс и возвращает только заголовки
//        }
//        else if (_session->request->GetMethod() == "PUT"
//                 || _session->request->GetMethod() == "OPTIONS"
//                 || _session->request->GetMethod() == "CONNECT"
//                 || _session->request->GetMethod() == "TRACE"
//                 || _session->request->GetMethod() == "PATCH") {
//            throw NotImplemented("Method " + _session->request->GetMethod() + " is not implemented",
//                                 _session->server_instance);
//        }
//        else {
//            throw MethodNotAllowed("Method not allowed", _session->server_instance);
//        }
    }
    catch (const HttpException& e) {
        _session->state = ConnectionState::ResponseToClient;
        _session->SendErrorDataToClient(e.GetErrorResponse());
        LOG_INFO("HttpException: ", e.what());
    }
    catch (const std::exception& e) {
        LOG_ERROR("Unexpected exception: ", e.what());
        /// need to close connection
    }

}
