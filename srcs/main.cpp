#include "Config.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        /// TODO make logger and call log_error
        exit(EXIT_FAILURE);
    }

    CommonConfig config;
    config.Load(argv[1]);

    return 0;
}