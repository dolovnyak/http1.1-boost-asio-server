#include "Event.h"
#include "SharedPtr.h"

#include <queue>

class HandleHttpRequestEvent : public Event {
public:
    HandleHttpRequestEvent(const SharedPtr<Connection>& connection,
                           const SharedPtr<std::string>& raw_request_part,
                           std::queue<Event>* event_queue);

    void Process();

    ~HandleHttpRequestEvent();

private:
    SharedPtr<Connection> _connection;
    SharedPtr<std::string> _raw_request_part;
    std::queue<Event>* _event_queue;
};


