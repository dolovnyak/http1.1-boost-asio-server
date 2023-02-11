#pragma once

#include "Request.h"

#define EXECVE_ERROR "Execve error"

namespace Http {

struct EnvironmentVariable {
    const std::string key;
    const std::string value;

    EnvironmentVariable(std::string key, std::string value)
            : key(std::move(key)), value(std::move(value)) {}
};

class CgiHandler {
public:
    static std::string
    Handle(const std::shared_ptr<Request>& request, const std::shared_ptr<Location>& matched_location,
           const std::string& content_type, const std::string& script_path,
           const std::vector<EnvironmentVariable>& additional_variables);
};

}