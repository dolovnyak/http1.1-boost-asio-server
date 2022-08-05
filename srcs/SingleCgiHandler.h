#pragma once

#include <string>
#include <unistd.h>

#include "Config.h"

class SingleCgiHandler {
    void SetConfig(const Config& config);
    std::string Handle();
};
