#include "Event.h"

#include <utility>

Event::Event(EventType type,
            std::function<void()> process_function)
        : _type(type),
          _process_function(std::move(process_function)) {}

void Event::Process() {
    LOG_INFO("Process event: ", GetName());
    _process_function();
}

const std::string& EventTypeToString(EventType event_type) {
    static std::string kEventsNames[] = {
            "ParseHttpRequest"
    };

    return kEventsNames[static_cast<int>(event_type)];
}

const std::string& Event::GetName() const {
    return EventTypeToString(_type);
}
