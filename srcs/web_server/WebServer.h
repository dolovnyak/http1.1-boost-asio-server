#pragma once

#include "Config.h"

class WebServer {
public:
    void Setup(const Config& config);

private:
    Config _config;
};

