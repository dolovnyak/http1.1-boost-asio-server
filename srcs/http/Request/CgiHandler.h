#pragma once

#include "Request.h"

namespace Http {

class CgiHandler {
public:
    static std::string Handle(const std::shared_ptr<Request>& request, const std::shared_ptr<Location>& matched_location, const std::string& content_type);
};

}