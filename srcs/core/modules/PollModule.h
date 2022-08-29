#pragma once

#include "Config.h"
#include "ServerInfo.h"
#include "Event.h"
#include "SharedPtr.h"

#include <unordered_map>
#include <queue>
#include <cstdint>

class PollModule {
public:
    bool Setup(const Config& config, std::queue<SharedPtr<Event> >* event_queue);

    void ProcessEvents(int timeout);

private:
    void ProcessRead(int index);

    void ProcessWrite(int index);

    void ProcessNewConnection(int index);

    void ProcessCompress();

private: /// for connection communication
    void SendDataToClient(int index);

    void CloseConnection(int index);

    friend class Session<PollModule>; /// TODO maybe del

private:
    int _poll_fds_number;
    struct pollfd* _poll_fds;
    bool _should_compress;

    std::unordered_map<int, SharedPtr<ServerInfo> > _servers;
    std::unordered_map<int, SharedPtr<Session<PollModule> > > _connections;
    std::queue<SharedPtr<Event> >* _event_queue;
};
