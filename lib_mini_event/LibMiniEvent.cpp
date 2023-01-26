#include "LibMiniEvent.h"

void LibMiniEvent::PreProcessEvents() {
    /// correct update sessions map and internal fds array.
    _core.PreProcessEvents(_mini_sessions, _sessions_to_add, _sessions_to_change, _sessions_to_delete);

    _sessions_to_add.clear();
    _sessions_to_change.clear();
    _sessions_to_delete.clear();
}

void LibMiniEvent::ProcessEvents(int timeout) {
    /// process inner read/write/hang/error events. which will spawn MiniEvents to _mini_events_queue.
    _core.ProcessEvents(_mini_sessions, _mini_events_queue, timeout);
}

void LibMiniEvent::PostProcessEvents() {
    /// process MiniEvents from _mini_events_queue, which will spawn Events to _events_queue or update sessions by AddSession/DeleteSession/SetPendingEventsOnSession
    while (!_mini_events_queue.empty()) {
        _mini_events_queue.front()->Process();
        _mini_events_queue.pop();
    }
}

void LibMiniEvent::AddSession(IMiniSession* session, int pending_events) {
    _sessions_to_add.push_back(std::make_pair(session, pending_events));
}

void LibMiniEvent::SetPendingEventsOnSession(IMiniSession* session, int pending_events) {
    _sessions_to_change.push_back(std::make_pair(session, pending_events));
}

void LibMiniEvent::DeleteSession(IMiniSession* session) {
    session->available = false;
    _sessions_to_delete.push_back(session);
}