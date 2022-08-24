#pragma once

#include "Event.h"
#include "utilities.h"

#include <queue>

class ProcessIncomingDataEvent : public Event {
public:
    ProcessIncomingDataEvent(const SharedPtr<HttpConnection>& connection,
                             std::queue<SharedPtr<Event> >* event_queue);

    const std::string& GetName() const OVERRIDE;

    void Process() OVERRIDE;

    ~ProcessIncomingDataEvent();

private:
    SharedPtr<HttpConnection> _connection;
    std::queue<SharedPtr<Event> >* _event_queue;
};
