#pragma once

#include "Config.h"
#include "Event.h"
#include "SharedPtr.h"

#include <unordered_map>
#include <queue>
#include <cstdint>

class PollModule {
public:
    PollModule(const SharedPtr<Config>& config, std::queue<SharedPtr<Event> >* event_queue);

    ~PollModule();

    void ProcessEvents(int timeout);

private:
    void ProcessRead(int index);

    void ProcessWrite(int index);

    void ProcessNewConnection(int index);

    void ProcessCompress();

private: /// for connection communication

    void CloseSocket(int index);

    void SendDataToClient(int index);

    friend class HttpSession<PollModule>;

private:
    SharedPtr<Config> _config;
    std::queue<SharedPtr<Event> >* _event_queue;
    std::unordered_map<int, SharedPtr<ServerConfig> > _servers;
    std::unordered_map<int, SharedPtr<HttpSession<PollModule> > > _sessions;

    int _poll_fds_number;
    struct pollfd* _poll_fds;
    bool _should_compress;
    size_t _read_buffer_size;
    char* _read_buffer;
};
