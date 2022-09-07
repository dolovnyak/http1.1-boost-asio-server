#include "Config.h"

#include <unordered_map>

struct SocketFd {
    explicit SocketFd(int value) : _fd(value) {}

    operator int() const { return _fd; }

    bool operator==(const SocketFd& socket_fd) const { return _fd == socket_fd._fd; }

private:
    int _fd;
};

template<>
struct std::hash<SocketFd> {
    std::size_t operator()(SocketFd socket) const {
        return std::hash<int>()(socket);
    }
};

int SetupServerSocket(SharedPtr<ServerConfig> server_config, SharedPtr<Config> config);
