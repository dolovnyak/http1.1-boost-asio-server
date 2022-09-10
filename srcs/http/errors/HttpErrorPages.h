#pragma once

#include "Http.h"
#include "Optional.h"
#include "SharedPtr.h"
#include "Config.h"

const std::string& GetHttpErrorPageByCode(Http::Code code, Optional<SharedPtr<ServerConfig> > server_config);
