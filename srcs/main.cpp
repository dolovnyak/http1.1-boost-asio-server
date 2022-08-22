#include "WebServer.h"
#include "utilities/Logging.h"
#include "PollModule.h"

int main(int argc, char** argv) {

//    const auto p1 = std::chrono::system_clock::now();
//
//    const auto p2 = std::chrono::system_clock::now();
//    auto p3 = p2 - p1;
//
//    std::cout << "duration " << std::chrono::duration_cast<std::chrono::microseconds>(p3).count() << std::endl;


    if (argc != 2) {
        LOG_IMPORTANT("Usage: ./webserver <config_file_path>");
        exit(EXIT_FAILURE);
    }

    Config config;
    if (!config.Load(argv[1])) {
        LOG_ERROR("Failed to load config");
        exit(EXIT_FAILURE);
    }

    WebServer<PollModule> web_server;
    if (!web_server.Setup(config)) {
        LOG_ERROR("Failed to setup web server");
        exit(EXIT_FAILURE);
    }

    web_server.Run();
    return 0;
}
