#pragma once

#include "Config.h"

#define JSON_READ_BUFFER_SIZE 4096

class ConfigParser {
public:
    static Config Parse(const char* path);
};

