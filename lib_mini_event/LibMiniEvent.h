#pragma once

#include "MiniSession.h"
#include "mini_helpers.h"

typedef std::pair<SharedPtr<IMiniSession>, std::vector<InternalEventType> > PairSessionEvents;

class LibMiniEvent {
public:
    void PreProcessEvents();

    void ProcessEvents(int timeout);

    void PostProcessEvents();

public:
    void AddSession(IMiniSession* session, std::vector<InternalEventType> pending_events);

    void DeleteSession(IMiniSession* session);

    void SetPendingEventsOnSession(IMiniSession* session, std::vector<InternalEventType> pending_events);

private:
    std::unordered_map<SocketFd, SharedPtr<IMiniSession> > _mini_sessions;

    std::vector<PairSessionEvents> _sessions_to_add;

    std::vector<PairSessionEvents> _sessions_to_change;

    std::vector<SharedPtr<IMiniSession> > _sessions_to_delete;

    std::queue<SharedPtr<MiniEvent> > _mini_events_queue;

#ifdef _POLL
    PollModule _core;
#elif _KQUEUE
    KqueueModule _core;
#elif _EPOLL
    EpollModule _core;
#endif
};
