#include "WebServerManager.h"
#include "ConfigParser.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        LOG_ERROR("Usage: ./webserver <config_file_path>");
        exit(EXIT_FAILURE);
    }

    try {
        std::shared_ptr<Config> config = std::make_shared<Config>(ConfigParser::Parse(argv[1]));

        WebServerManager web_server(config);
        web_server.Run();
    }
    catch (const std::exception& e) {
        LOG_ERROR("Failed to run web server:\n", e.what());
        exit(EXIT_FAILURE);
    }
    return 0;
}
