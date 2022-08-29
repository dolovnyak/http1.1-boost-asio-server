#include "Request.h"
#include <sstream>
#define _DEBUG
#include "../../utilities/Logging.h"
#include "../../core/ServerInfo.h"

HttpVersion ParseHttpVersion(const std::string& raw_http_version, const SharedPtr<ServerInfo>& server_instance_info) {
    std::vector<std::string> tokens = SplitString(raw_http_version, "/");
    if (tokens.size() != 2 || tokens[0] != "HTTP") {
        throw BadHttpVersion("Incorrect HTTP version", server_instance_info);
    }

    std::vector<std::string> version_tokens = SplitString(tokens[1], ".");
    if (version_tokens.size() != 2) {
        throw BadHttpVersion("Incorrect HTTP version", server_instance_info);
    }

    try {
        HttpVersion http_version = HttpVersion(
                ParsePositiveInt(version_tokens[0]),
                ParsePositiveInt(version_tokens[1])
        );
        return http_version;
    }
    catch (const std::exception& e) {
        throw BadHttpVersion("Incorrect HTTP version" + std::string(e.what()), server_instance_info);
    }
}

int main() {
    yy::location loc;

    std::stringstream ss;
    yy::Lexer lexer;
    lexer.switch_streams(&ss, nullptr);

    {
        SharedPtr<ServerInfo> server_instance_info = MakeShared(ServerInfo(2, "kabun"));
        Request request(server_instance_info);
        LOG_START_TIMER("MAIN_OLD");

        std::string raw_request = "!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz / HTTP/1.1";

        std::vector<std::string> tokens = SplitString(raw_request, DELIMITERS);
        if (tokens.size() != 3) {
            throw BadFirstLine("Incorrect first line", server_instance_info);
        }
        request.method = tokens[0];
        for (char c: request.method) {
            if (!isalnum(c) && c != '!' && c != '#' && c != '$' && c != '%' && c != '&' && c != '\'' && c != '*' &&
                c != '+' && c != '-' && c != '.' && c != '^' && c != '_' && c != '`' && c != '|' && c != '~') {
                throw BadFirstLine("Incorrect first line", server_instance_info);
            }
        }

        request.resource_target = tokens[1];
        if (request.resource_target[0] != '/') {
            throw BadFirstLine("Incorrect first line", server_instance_info);
        }

        request.http_version = ParseHttpVersion(tokens[2], server_instance_info);

        LOG_INFO("Method: ", request.method);
        LOG_INFO("Resource target: ", request.resource_target);
        LOG_INFO("HTTP version: ", request.http_version.major, ".", request.http_version.minor);

        LOG_TIME();
    }


    {
        SharedPtr<ServerInfo> server_instance_info = MakeShared(ServerInfo(2, "kabun"));
        Request request(server_instance_info);
        LOG_START_TIMER("MAIN_NEW");

        ss << "!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz / HTTP/1.1";

        lexer.set_parse_state(yy::ParseState::FirstLine);

        yy::Parser parser(lexer, request);
        parser.parse();
        LOG_INFO("Method: ", request.method);
        LOG_INFO("Resource target: ", request.resource_target);
        LOG_INFO("HTTP version: ", request.http_version.major, ".", request.http_version.minor);

        LOG_TIME();
    }
}
