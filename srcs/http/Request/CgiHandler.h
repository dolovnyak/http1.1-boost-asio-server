#pragma once

#include "Request.h"
#include "Response.h"

#define EXECVE_ERROR "Execve error"

namespace Http {

struct EnvironmentVariable {
    const std::string key;
    const std::string value;

    EnvironmentVariable(std::string key, std::string value)
            : key(std::move(key)), value(std::move(value)) {}
};

class CgiHandler {
private:
    std::shared_ptr<Request> _request{};
    std::string _content_type{};
    std::string _script_path{};
    std::vector<EnvironmentVariable> _additional_variables{};
    size_t _parsed_size = 0;
    std::string _raw_result{};


public:
    CgiHandler(const std::shared_ptr<Request>& request,
               const std::string& content_type, const std::string& script_path,
               const std::vector<EnvironmentVariable>& additional_variables);

    std::shared_ptr<Response> Handle();

private:
    std::vector<EnvironmentVariable> CreateCgiEnvironment();

    std::shared_ptr<Response> ParseHandleResult();

    bool ParseHeaders(std::vector<Header>& headers);
};

}