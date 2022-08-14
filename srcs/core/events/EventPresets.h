#pragma once

#include "Event.h"
#include "Request.h"

#include <queue>

class EventPresets {
public:
    static Event ParseHttpRequest(const std::shared_ptr<Connection>& connection,
                                  const std::shared_ptr<std::string>& raw_request,
                                  std::queue<Event>* event_queue);

    static Event ProcessHttpRequest(const std::shared_ptr<Connection>& connection,
                                    std::queue<Event>* event_queue);
};
