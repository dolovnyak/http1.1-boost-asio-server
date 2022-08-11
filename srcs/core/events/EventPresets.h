#pragma once

#include "Event.h"

class EventPresets {
public:
    static Event HttpParseRequestEvent(const ServerInstance& server_instance,
                                       const ConnectionInstance& connection_instance,
                                       const std::shared_ptr<std::string>& raw_request);
};
