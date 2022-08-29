#pragma once

#include "Event.h"
#include "HttpException.h"
#include "utilities.h"

#include <queue>

template<class CoreModule>
class ProcessRequestEvent : public Event {
public:
    ProcessRequestEvent(const SharedPtr<Session<CoreModule> >& connection,
                        std::queue<SharedPtr<Event> >* event_queue)
            : _connection(connection), _event_queue(event_queue) {}

    ~ProcessRequestEvent() {}

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;


private:
    SharedPtr<Session<CoreModule> > _connection;
    std::queue<SharedPtr<Event> >* _event_queue;
};


template<class CoreModule>
const std::string& ProcessRequestEvent<CoreModule>::GetName() const {
    static std::string kName = "ProcessRequestEvent";
    return kName;
}

template<class CoreModule>
void ProcessRequestEvent<CoreModule>::Process() {
    try {
        if (!_connection->available) {
            LOG_INFO(GetName(), " on closed connection");
            return;
        }
        if (_connection->state != ConnectionState::ProcessRequest) {
            LOG_INFO(GetName(), " on wrong connection state");
            return;
        }

        /// analyze resource here and check if it valid and exists, if no throw http exception

//        if (_connection->request->GetMethod() == "GET") {
//            _connection->state = ConnectionState::HandleRequest;
//        }
//        else if (_connection->request->GetMethod() == "POST") {
//            /// resource should be script
//            _connection->state = ConnectionState::HandleRequest;
//        }
//        else if (_connection->request->GetMethod() == "DELETE") {
//            /// resource should be script
//            _connection->state = ConnectionState::HandleRequest;
//        }
//        else if (_connection->request->GetMethod() == "HEAD") {
//            _connection->state = ConnectionState::HandleRequest;
//        }
//        else if (_connection->request->GetMethod() == "PUT"
//                 || _connection->request->GetMethod() == "OPTIONS"
//                 || _connection->request->GetMethod() == "CONNECT"
//                 || _connection->request->GetMethod() == "TRACE"
//                 || _connection->request->GetMethod() == "PATCH") {
//            throw NotImplemented("Method " + _connection->request->GetMethod() + " is not implemented",
//                                 _connection->server_instance);
//        }
//        else {
//            throw MethodNotAllowed("Method not allowed", _connection->server_instance);
//        }
        /// Analyze method and resource -> will return handler type
        /// Run handler -> will return response

        /// посмотреть какой метод и поддерживаемый ли он
        /// распарсить ресурс
        /// если ресурс ведет к файлу, который лежит в папке cgi (или возможно чутка по-другому если будет fastCgi) -> CgiHandler
        /// если к обычному файлу -> FileHandler

//        _connection->ProcessIncomingData();
//        _connection->SendProcessedDataToClient();
    }
    catch (const HttpException& e) {
        _connection->state = ConnectionState::ResponseToClient;
        _connection->SendErrorDataToClient(e.GetErrorResponse());
        LOG_INFO("HttpException: ", e.what());
    }
    catch (const std::exception& e) {
        LOG_ERROR("Unexpected exception: ", e.what());
        /// need to close connection
    }

}
