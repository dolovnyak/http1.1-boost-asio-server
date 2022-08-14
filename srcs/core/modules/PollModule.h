#pragma once

#include "Config.h"
#include "ServerInstance.h"
#include "core/events/Event.h"
#include "HttpModule.h"

#include <unordered_map>
#include <queue>
#include <cstdint>

class PollModule {
public:
    bool Setup(const Config& config, std::queue<Event>* event_queue);

    void ProcessEvents(int timeout);

private:
    void ProcessRead(int index);

    void ProcessWrite(int index);

    void ProcessNewConnection(int index);

    void ProcessCompress();

    void CloseConnection(int index);

private:
    int _poll_fds_number;
    struct pollfd* _poll_fds;
    bool _should_compress;

    std::unordered_map<int32_t, std::shared_ptr<ServerInstance>> _servers;
    std::unordered_map<int32_t, std::shared_ptr<Connection>> _connections;
    std::queue<Event>* _event_queue;
};