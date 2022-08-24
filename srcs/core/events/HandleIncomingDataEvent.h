#pragma once

#include "Event.h"
#include "SharedPtr.h"
#include "utilities.h"

#include <queue>

class HandleIncomingDataEvent : public Event {
public:
    HandleIncomingDataEvent(const SharedPtr<HttpConnection>& connection,
                            const SharedPtr<std::string>& incoming_data,
                            std::queue<SharedPtr<Event> >* event_queue);

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;

    ~HandleIncomingDataEvent();

private:
    SharedPtr<HttpConnection> _connection;
    SharedPtr<std::string> _incoming_data;
    std::queue<SharedPtr<Event> >* _event_queue;
};
