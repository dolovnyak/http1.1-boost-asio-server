#include "HttpConnection.h"
#include "Logging.h"
#include "HttpException.h"

HandleIncomingDataStatus::Status HttpConnection::HandleIncomingData(const SharedPtr<std::string>& incoming_data) {
    if (!_available) {
        LOG_INFO("HandleIncomingData on closed connection");
        return HandleIncomingDataStatus::NotAvailable;
    }
    if (_state != ConnectionState::HandleRequest) {
        LOG_INFO("HandleIncomingData on connection in wrong state");
        return HandleIncomingDataStatus::NotAvailable;
    }

    RequestHandleStatus::Status status = request->Handle(incoming_data);

    switch (status) {
        case RequestHandleStatus::Finish:
            _state = ConnectionState::ProcessRequest;
            return HandleIncomingDataStatus::Finish;

        case RequestHandleStatus::WaitMoreData:
            return HandleIncomingDataStatus::WaitMoreData;
    }
}

void HttpConnection::ProcessIncomingData() {
    if (!_available) {
        LOG_INFO("ProcessIncomingData on closed connection");
        return;
    }
    if (_state != ConnectionState::ProcessRequest) {
        LOG_INFO("ProcessIncomingData on connection in wrong state");
        return;
    }

    /// Analyze method and resource -> will return handler type
    /// Run handler -> will return response

    /// посмотреть какой метод и поддерживаемый ли он
    /// распарсить ресурс
    /// если ресурс ведет к файлу, который лежит в папке cgi (или возможно чутка по-другому если будет fastCgi) -> CgiHandler
    /// если к обычному файлу -> FileHandler
    _state = ConnectionState::ResponseToClient;
}

void HttpConnection::SendProcessedDataToClient() {
    if (!_available) {
        LOG_INFO("SendProcessedDataToClient on closed connection");
        return;
    }
    if (_state != ConnectionState::ResponseToClient) {
        LOG_INFO("Invoke SendProcessedDataToClient when response is not sent to client");
        return;
    }
}

void HttpConnection::SendErrorDataToClient(const SharedPtr<Response>& response) {
    if (!_available) {
        LOG_INFO("SendErrorDataToClient on closed connection");
        return;
    }
    _state = ConnectionState::ResponseToClient;
    /// нужно поставить poll ивент на запись и пометить, что конекшен должен быть закрыт после записи.
}

