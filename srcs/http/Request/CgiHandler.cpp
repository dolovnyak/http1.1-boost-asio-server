#include "CgiHandler.h"
#include "Exception.h"

#include <unistd.h>
#include <array>

namespace Http {

namespace {

std::vector<EnvironmentVariable>
CreateCgiEnvironment(const std::shared_ptr<Request>& request, const std::shared_ptr<Location>& matched_location,
                     const std::string& content_type, const std::string& script_path) {
    std::vector<EnvironmentVariable> env =
            {{"REDIRECT_STATUS",   "200"},
             {"GATEWAY_INTERFACE", "CGI/1.1"},
             {"SCRIPT_NAME",       script_path},
             {"SCRIPT_FILENAME",   script_path},
             {"REQUEST_METHOD",    ToString(request->http_method)},
             {"CONTENT_LENGTH",    std::to_string(request->body.length())},
             {"CONTENT_TYPE",      content_type},
             {"PATH_INFO",         request->target.path},
             {"PATH_TRANSLATED",   request->target.path},
             {"QUERY_STRING",      request->target.query_string},
             {"REQUEST_URI",       request->target.path + request->target.query_string},
             {"SERVER_NAME",       request->server_config->name},
             {"SERVER_PORT",       std::to_string(request->server_config->port)},
             {"SERVER_PROTOCOL",   ToString(request->http_version)},
             {"SERVER_SOFTWARE", WEBSERVER_NAME}};

    return env;
}

}

std::string
CgiHandler::Handle(const std::shared_ptr<Request>& request, const std::shared_ptr<Location>& matched_location,
                   const std::string& content_type, const std::string& script_path,
                   const std::vector<EnvironmentVariable>& additional_variables) {
    std::vector<EnvironmentVariable> environment = CreateCgiEnvironment(request, matched_location, content_type,
                                                                        script_path);
    for (const auto& variable : additional_variables) {
        environment.emplace_back(variable);
    }

    std::vector<const char*> env(environment.size() + 1);
    std::vector<std::string> env_variables(environment.size());
    for (size_t i = 0; i < environment.size(); ++i) {
        env_variables[i] = environment[i].key + "=" + environment[i].value;
        env[i] = env_variables[i].c_str();
    }
    env[environment.size()] = nullptr;
    char** env_to_script = (char**) (&env[0]);  /// it's scary but it's necessary

    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);

    int fd_in = fileno(tmpfile());
    int fd_out = fileno(tmpfile());

    write(fd_in, request->body.c_str(), request->body.size());
    lseek(fd_in, 0, SEEK_SET);

    pid_t pid = fork();
    if (pid == -1) {
        throw InternalServerError("Start cgi error", request->server_config);
    }
    else if (pid == 0) {
        dup2(fd_in, STDIN_FILENO);
        dup2(fd_out, STDOUT_FILENO);
        execve(script_path.c_str(), nullptr, env_to_script);
        std::string error = std::string(EXECVE_ERROR) + "\r\n\r\n";
        write(STDOUT_FILENO, error.c_str(), error.size());
        exit(-1);
    }
    else {
        char buffer[READ_BUFFER_SIZE];

        waitpid(pid, nullptr, 0);
        lseek(fd_out, 0, SEEK_SET);

        std::string result;
        size_t read_size;
        while ((read_size = read(fd_out, buffer, READ_BUFFER_SIZE - 1)) > 0) {
            result += std::string(buffer, read_size);
            memset(buffer, 0, READ_BUFFER_SIZE);
        }

        dup2(save_stdin, STDIN_FILENO);
        dup2(save_stdout, STDOUT_FILENO);
        close(fd_in);
        close(fd_out);
        close(save_stdin);
        close(save_stdout);
        return result;
    }

}

}