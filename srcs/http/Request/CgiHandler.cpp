#include "CgiHandler.h"
#include "Exception.h"

#include <unistd.h>
#include <array>

namespace Http {

struct EnvironmentVariable {
    const std::string key;
    const std::string value;

    EnvironmentVariable(std::string key, std::string value)
            : key(std::move(key)), value(std::move(value)) {}
};

namespace {

std::vector<EnvironmentVariable>
CreateCgiEnvironment(const std::shared_ptr<Request>& request, const std::shared_ptr<Location>& matched_location,
                     const std::string& content_type) {
    return {{"REDIRECT_STATUS",   "200"},
            {"GATEWAY_INTERFACE", "CGI/1.1"},
            {"SCRIPT_NAME",       matched_location->cgi_path.value()},
            {"SCRIPT_FILENAME",   matched_location->cgi_path.value()},
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
}

}

std::string
CgiHandler::Handle(const std::shared_ptr<Request>& request, const std::shared_ptr<Location>& matched_location,
                   const std::string& content_type) {
    std::vector<EnvironmentVariable> environment = CreateCgiEnvironment(request, matched_location, content_type);

    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);

    int fd_in = fileno(tmpfile());
    int fd_out = fileno(tmpfile());

//    write(fd_in, _body.c_str(), _body.size());
//    lseek(fd_in, 0, SEEK_SET);


    pid_t pid = fork();
    if (pid == -1) {
        throw InternalServerError("Start cgi error", request->server_config);
    }
    else if (pid == 0) {
//        char* const* nll = NULL;

        dup2(fd_in, STDIN_FILENO);
        dup2(fd_out, STDOUT_FILENO);
        execve(matched_location->cgi_path->c_str(), nullptr, nullptr);
        write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
        exit(-1);
    }
    else {
        char buffer[READ_BUFFER_SIZE];

        waitpid(pid, nullptr, 0);
        lseek(fd_out, 0, SEEK_SET);

        std::string body;
        size_t read_size;
        while ((read_size = read(fd_out, buffer, READ_BUFFER_SIZE - 1)) > 0) {
            body += std::string(buffer, read_size);
            memset(buffer, 0, READ_BUFFER_SIZE);
        }

        dup2(save_stdin, STDIN_FILENO);
        dup2(save_stdout, STDOUT_FILENO);
        close(fd_in);
        close(fd_out);
        close(save_stdin);
        close(save_stdout);
        return body;
    }

//    for (size_t i = 0; env[i]; i++)
//        delete[] env[i];
//    delete[] env;
//
//    if (!pid)
//        exit(0);

}

}