#include "WebServerManager.h"
#include "ConfigParser.h"

int main(int argc, char** argv) {
    auto stdout_sink = std::make_shared<StdoutSink>();
    Logger::AddSink(stdout_sink);

    auto file_sink = std::make_shared<FileSink>("logs.txt");
    Logger::AddSink(file_sink);

    stdout_sink->SetLevel(static_cast<LogLevel>(STDOUT_LOG_LEVEL));
    file_sink->SetLevel(static_cast<LogLevel>(FILE_LOG_LEVEL));

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
