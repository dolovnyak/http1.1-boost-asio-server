#pragma once

#include "Http.h"
#include "Config.h"

std::string GetHttpErrorPageByCode(Http::Code code, const std::shared_ptr<ServerConfig>& server_config);
