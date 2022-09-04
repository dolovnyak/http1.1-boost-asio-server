#include "WebServer.h"
#include "utilities/Logging.h"
#include "PollModule.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        LOG_ERROR("Usage: ./webserver <config_file_path>");
        exit(EXIT_FAILURE);
    }

    try {
        SharedPtr<Config> config = MakeShared(Config());
        if (!config->Load(argv[1])) {
            LOG_ERROR("Failed to load config");
            exit(EXIT_FAILURE);
        }

        WebServer<PollModule> web_server(config);
        web_server.Run();
    }
    catch (const std::exception& e) {
        LOG_ERROR("Failed to run web server: %s", e.what());
        exit(EXIT_FAILURE);
    }
    return 0;
}
