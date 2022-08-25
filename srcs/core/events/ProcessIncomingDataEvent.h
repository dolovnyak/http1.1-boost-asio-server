#pragma once

#include "Event.h"
#include "HttpException.h"
#include "utilities.h"

#include <queue>

template<class CoreModule>
class ProcessIncomingDataEvent : public Event {
public:
    ProcessIncomingDataEvent(const SharedPtr<Connection<CoreModule>>& connection,
                             std::queue<SharedPtr<Event> >* event_queue)
            : _connection(connection), _event_queue(event_queue) {}

    ~ProcessIncomingDataEvent() {}

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;


private:
    SharedPtr<Connection<CoreModule>> _connection;
    std::queue<SharedPtr<Event> >* _event_queue;
};


template<class CoreModule>
const std::string& ProcessIncomingDataEvent<CoreModule>::GetName() const {
    static std::string kName = "ProcessIncomingDataEvent";
    return kName;
}

template<class CoreModule>
void ProcessIncomingDataEvent<CoreModule>::Process() {
    try {
        if (!_connection->available) {
            LOG_INFO(GetName() + " on closed connection");
            return;
        }
        if (_connection->state != ConnectionState::ProcessRequest) {
            LOG_INFO(GetName() + " on wrong connection state");
            return;
        }
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
