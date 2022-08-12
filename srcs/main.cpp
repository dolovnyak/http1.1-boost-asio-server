#include "WebServer.h"
#include "utilities/logging.h"

#ifdef EPOLL
#include "EpollModule.h"
using ServerInstance = WebServer<EpollModule>;
#elif POLL
#include "PollModule.h"
using Server = WebServer<PollModule>;
#endif

int main(int argc, char** argv) {
    if (argc != 2) {
        LOG_IMPORTANT("Usage: ./webserver <config_file_path>");
        exit(EXIT_FAILURE);
    }

    Config config;
    if (!config.Load(argv[1])) {
        LOG_ERROR("Failed to load config");
        exit(EXIT_FAILURE);
    }

    Server web_server;
    if (!web_server.Setup(config)) {
        LOG_ERROR("Failed to setup web server");
        exit(EXIT_FAILURE);
    }

    web_server.Run();
    return 0;
}