#define private public
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

        request.target.path = tokens[1];
        if (request.target.path[0] != '/') {
            throw BadFirstLine("Incorrect first line", server_instance_info);
        }

        request.http_version = ParseHttpVersion(tokens[2], server_instance_info);

        LOG_INFO("Method: ", request.method);
        LOG_INFO("Resource target: ", request.target.path);
        LOG_INFO("HTTP version: ", request.http_version.major, ".", request.http_version.minor);

        LOG_TIME();
    }


    {
        SharedPtr<ServerInfo> server_instance_info = MakeShared(ServerInfo(2, "kabun"));
        Request request(server_instance_info);
        LOG_START_TIMER("MAIN_NEW");

        std::string first_line = "!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz /a.lua??\?\?////a=asd//\?\?/??kabun HTTP/1.1";

        request._parser.Parse(first_line, 0, first_line.size(), yy::ParseState::FirstLine);

        LOG_WARNING("first line parsed");

        std::string header = "Host:     kabun.com   a       ";
        request._parser.Parse(header, 0, header.size(), yy::ParseState::Header);

        LOG_INFO("Method: ", request.method);
        LOG_INFO("Resource target: ", request.target.path);
        LOG_INFO("Resource query: ", request.target.query);
        LOG_INFO("HTTP version: ", request.http_version.major, ".", request.http_version.minor);
        LOG_INFO("Headers size: ", request.headers.size());
        for (auto& header: request.headers) {
            LOG_INFO("Header: ", header.first, ", ", header.second[0]);
        }

        LOG_TIME();
    }
}
