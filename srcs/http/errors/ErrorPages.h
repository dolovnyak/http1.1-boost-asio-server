#pragma once

#include "Http.h"
#include "Config.h"

namespace Http {
std::string GetErrorPageByCode(Http::Code code, const std::shared_ptr<ServerConfig>& server_config);
}
