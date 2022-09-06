#include "PollModule.h"
#include "WebServer.h"

#include <unistd.h>
void check_cgi() {
    char** env = (char**)malloc(2 * sizeof(char*));

    env[0] = strdup("AUTH_TYPE=kabun");
    env[1] = NULL;

    char * const * nll = NULL;
    int err = execve("/Users/sbecker/Desktop/projects/webserver-42/examples/cgi_checker/simple_cgi.py", nll, env);

        std::cout << "error: " << err << std::endl;
}

int main(int argc, char** argv) {

    check_cgi();

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
