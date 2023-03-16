#include "CgiHandler.h"
#include "Exception.h"

#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

namespace Http {

namespace {
std::string ToCgiKey(const std::string& key) {
    std::string res = "HTTP_";
    for (char c: key) {
        if (c == '-') {
            res.push_back('_');
        }
        else {
            res.push_back(toupper(c));
        }
    }
    return res;
}
}

std::vector<EnvironmentVariable> CgiHandler::CreateCgiEnvironment() {
    std::vector<EnvironmentVariable> env =
            {{"REDIRECT_STATUS",   "200"},
             {"GATEWAY_INTERFACE", "CGI/1.1"},
             {"SCRIPT_NAME",       _script_path},
             {"SCRIPT_FILENAME",   _script_path},
             {"REQUEST_METHOD",    ToString(_request->http_method)},
             {"CONTENT_LENGTH",    std::to_string(_request->body.length())},
             {"CONTENT_TYPE",      _content_type},
             {"PATH_INFO",         _request->target.path},
             {"PATH_TRANSLATED",   _request->target.path},
             {"QUERY_STRING",      _request->target.query_string},
             {"REQUEST_URI",       _request->target.path + _request->target.query_string},
             {"SERVER_NAME",       _request->server_config->name},
             {"SERVER_PORT",       std::to_string(_request->server_config->port)},
             {"SERVER_PROTOCOL",   ToString(_request->http_version)},
             {"SERVER_SOFTWARE", WEBSERVER_NAME}};

    for (const auto& header: _request->http_headers) {
        for (const auto& header_var: header.second) {
            env.emplace_back(ToCgiKey(header.first), header_var);
        }
    }
    return env;
}


std::shared_ptr<Response> CgiHandler::Handle() {

    if (!IsExecutableFile(_script_path)) {
        throw NotFound("Script not found", _request->server_config);
    }

    std::vector<EnvironmentVariable> environment = CreateCgiEnvironment();
    for (const auto& variable: _additional_variables) {
        environment.emplace_back(variable);
    }

    std::vector<const char*> env(environment.size() + 1);
    std::vector<std::string> env_variables(environment.size());
    for (size_t i = 0; i < environment.size(); ++i) {
        env_variables[i] = environment[i].key + "=" + environment[i].value;
        env[i] = env_variables[i].c_str();
    }
    env[environment.size()] = nullptr;
    char** env_to_script = (char**) (&env[0]);
    char* const* null_ptr = nullptr;

    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);

    int fd_in = fileno(tmpfile());
    int fd_out = fileno(tmpfile());

    write(fd_in, _request->body.c_str(), _request->body.size());
    lseek(fd_in, 0, SEEK_SET);

    pid_t pid = fork();
    if (pid == -1) {
        throw InternalServerError("Start cgi error", _request->server_config);
    }
    else if (pid == 0) {
        dup2(fd_in, STDIN_FILENO);
        dup2(fd_out, STDOUT_FILENO);
        execve(_script_path.c_str(), null_ptr, env_to_script);
        std::string error = std::string(EXECVE_ERROR) + " errno is " + std::to_string(errno) + "\r\n\r\n";
        write(STDOUT_FILENO, error.c_str(), error.size());
        exit(-1);
    }
    else {
        char buffer[READ_BUFFER_SIZE];

        waitpid(pid, nullptr, 0);
        lseek(fd_out, 0, SEEK_SET);

        size_t read_size;
        while ((read_size = read(fd_out, buffer, READ_BUFFER_SIZE - 1)) > 0) {
            _raw_result += std::string(buffer, read_size);
            memset(buffer, 0, READ_BUFFER_SIZE);
        }

        dup2(save_stdin, STDIN_FILENO);
        dup2(save_stdout, STDOUT_FILENO);
        close(fd_in);
        close(fd_out);
        close(save_stdin);
        close(save_stdout);
        return ParseHandleResult();
    }

}

bool CgiHandler::ParseHeaders(std::vector<Header>& headers) {
    size_t header_end = _raw_result.find(CRLF, _parsed_size);

    if (header_end == _parsed_size) {
        /// two empty lines in a _raw_result, switch to body handle
        _parsed_size += CRLF_LEN;
        return true;
    }

    size_t key_end = FindInRange(_raw_result, ":", _parsed_size, header_end);
    if (key_end == std::string::npos) {
        throw InternalServerError("Incorrect header", _request->server_config);
    }
    std::string key = _raw_result.substr(_parsed_size, key_end - _parsed_size);
    if (key.empty() || !IsTcharString(key)) {
        throw InternalServerError("Incorrect header", _request->server_config);
    }
    std::string value = _raw_result.substr(key_end + 1, header_end - key_end - 1);
    if (value.empty()) {
        throw InternalServerError("Incorrect header", _request->server_config);
    }

    headers.emplace_back(key, value);
    _parsed_size = header_end + CRLF_LEN;
    return false;
}

std::shared_ptr<Response> CgiHandler::ParseHandleResult() {
    std::vector<Header> headers = {
            Header("Server", _request->server_config->name),
            Header("Date", GetCurrentDateTimeString())};

    while (!ParseHeaders(headers)) {}

    std::string body = _raw_result.substr(_parsed_size, _raw_result.size() - _parsed_size);

    headers.emplace_back("Content-Length", std::to_string(body.size()));

    bool content_type_was_founded = false;
    std::optional<int> status_code;
    std::optional<std::string> status;

    for (const auto& header: headers) {
        if (ToLower(header.key) == CONTENT_TYPE) {
            content_type_was_founded = true;
        }
        if (ToLower(header.key) == "status") {
            status_code = std::stoi(SplitString(header.value, " ").front());
            status = SplitString(header.value, " ").back();
        }
    }
    if (!content_type_was_founded) {
        headers.emplace_back("Content-Type", "text/html, charset=utf-8");
    }
    unsigned int code = status_code.value_or(static_cast<unsigned int>(Code::Accepted));

    return std::make_shared<Response>(
            ToHttpCode(code),
            status.value_or(ToString(ToHttpCode(code))),
            headers,
            body);
}


CgiHandler::CgiHandler(const std::shared_ptr<Request>& request,
                       const std::string& content_type, const std::string& script_path,
                       const std::vector<EnvironmentVariable>& additional_variables)
        : _request(request), _content_type(content_type), _script_path(script_path),
          _additional_variables(additional_variables) {}

}