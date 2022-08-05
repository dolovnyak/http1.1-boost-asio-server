#include "web-server/WebServer.h"

#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        /// TODO make logger and call log_error
        exit(EXIT_FAILURE);
    }

    try {
        Config config;
        config.Load(argv[1]);
    }
    catch (const std::exception& e) {
        /// TODO make logger
        std::cout << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}