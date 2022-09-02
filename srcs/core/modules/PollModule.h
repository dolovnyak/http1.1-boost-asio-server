#pragma once

#include "Config.h"
#include "Event.h"
#include "SharedPtr.h"
#include "Session.h"

#include <unordered_map>
#include <queue>
#include <cstdint>

class PollModule {
public:
    PollModule(const SharedPtr<Config>& config, std::queue<SharedPtr<Event> >* event_queue);

    ~PollModule();

    void ProcessEvents(int timeout);

    void SendDataToSocket(int socket_id);

    void AddSession(int socket, const SharedPtr<Session<PollModule> >& session);

private:
    void ProcessRead(int index);

    void ProcessWrite(int index);

    void ProcessNewHttpSessions(int index);

    void ProcessCompress();

    void CloseSocket(int socket_id);


private:
    SharedPtr<Config> _config;
    std::queue<SharedPtr<Event> >* _event_queue;
    std::unordered_map<int, SharedPtr<ServerConfig> > _servers;
    std::unordered_map<int, SharedPtr<Session<PollModule> > > _sessions;

    int _session_last_id;
    struct pollfd* _poll_fds;
    bool _should_compress;
    size_t _read_buffer_size;
    char* _read_buffer;
};
