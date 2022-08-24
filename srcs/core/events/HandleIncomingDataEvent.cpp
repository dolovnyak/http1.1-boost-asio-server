#include "HandleIncomingDataEvent.h"
#include "ProcessIncomingDataEvent.h"
#include "HttpException.h"

HandleIncomingDataEvent::HandleIncomingDataEvent(const SharedPtr<HttpConnection>& connection,
                                                 const SharedPtr<std::string>& incoming_data,
                                                 std::queue<SharedPtr<Event> >* event_queue)
        : _connection(connection),
          _incoming_data(incoming_data),
          _event_queue(event_queue) {}

HandleIncomingDataEvent::~HandleIncomingDataEvent() {}

const std::string& HandleIncomingDataEvent::GetName() const {
    static std::string kName = "HandleIncomingDataEvent";
    return kName;
}

void HandleIncomingDataEvent::Process() {
    try {
        HandleIncomingDataStatus::Status status = _connection->HandleIncomingData(_incoming_data);

        switch (status) {
            case HandleIncomingDataStatus::NotAvailable:
            case HandleIncomingDataStatus::WaitMoreData:
                break;

            case HandleIncomingDataStatus::Finish:
                _event_queue->push(MakeShared<Event>(new ProcessIncomingDataEvent(_connection, _event_queue)));
                break;
        }
    }
    catch (const HttpException& e) {
        _connection->SendErrorDataToClient(e.GetErrorResponse());
        LOG_INFO("HttpException: ", e.what());
    }
    catch (const std::exception& e) {
        LOG_ERROR("Unexpected exception: ", e.what());
    }
}
