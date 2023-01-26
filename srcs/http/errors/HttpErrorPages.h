#pragma once

#include "Http.h"
#include "Optional.h"
#include "SharedPtr.h"
#include "Config.h"

std::string GetHttpErrorPageByCode(Http::Code code, SharedPtr<ServerConfig> server_config);
