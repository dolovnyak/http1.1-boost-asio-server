#pragma once

#include "Config.h"
#include "ServerInstance.h"

class Poll {
public:
    std::vector<ServerInstance> Setup(const Config& config);

    void ProcessEvents(int timeout);

private:
    void ProcessRead(int index);

    void ProcessWrite(int index);

    void ProcessNewConnection(int index);

    void ProcessCompress();

    void CloseSocket(int index);

private:
    int _poll_fds_number;
    struct pollfd* _poll_fds;
    bool _should_compress;
    std::vector<ServerInstance> _server_instances;
};
