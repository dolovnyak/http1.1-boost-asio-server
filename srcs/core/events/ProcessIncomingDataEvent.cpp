#include "ProcessIncomingDataEvent.h"
#include "HttpException.h"

ProcessIncomingDataEvent::ProcessIncomingDataEvent(const SharedPtr<HttpConnection>& connection,
                                                   std::queue<SharedPtr<Event> >* event_queue)
    : _connection(connection), _event_queue(event_queue) {
}

ProcessIncomingDataEvent::~ProcessIncomingDataEvent() {}

const std::string& ProcessIncomingDataEvent::GetName() const {
    static std::string kName = "ProcessIncomingDataEvent";
    return kName;
}

void ProcessIncomingDataEvent::Process() {
    try {
        _connection->ProcessIncomingData();
        _connection->SendProcessedDataToClient();
    }
    catch (const HttpException& e) {
        _connection->SendErrorDataToClient(e.GetErrorResponse());
        LOG_INFO("HttpException: ", e.what());
    }
    catch (const std::exception& e) {
        LOG_ERROR("Unexpected exception: ", e.what());
    }
}
