#include <glob.h>
#include <vector>
#include <sys/stat.h>
#include <filesystem>

#include "AutoindexHandler.h"

namespace Http {

namespace {
typedef struct {
    std::string short_name;
    std::string full_name;
    std::string time_string;
} FileData;


std::vector<std::string> SplitString(const std::string& str, const std::string& delimiters) {
    std::vector<std::string> tokens;
    size_t pos = 0;

    while (true) {
        size_t token_start = str.find_first_not_of(delimiters, pos);
        if (token_start == std::string::npos) {
            return tokens;
        }

        size_t token_end = str.find_first_of(delimiters, token_start);
        if (token_end == std::string::npos) {
            tokens.push_back(str.substr(token_start));
            return tokens;
        }

        tokens.push_back(str.substr(token_start, token_end - token_start));
        pos = token_end + 1;
    }
}

std::string GetEndOfPath(const std::string& path) {
    std::vector<std::string> splited_string = SplitString(path, "/");
    std::string end_of_string = splited_string[splited_string.size() - 1];
    return end_of_string;
}

std::string GetFirstPartOfPath(const std::string& path) {
    std::vector<std::string> splited_string = SplitString(path, "/");
    std::string result;

    if (splited_string.empty()) {
        return "";
    }
    for (unsigned int i = 0; i < splited_string.size(); ++i) {
        if (i < splited_string.size() - 1) {
            result += "/" + splited_string[i];
        }

    }
    return result;
}

glob_t get_glob_result(const std::string& path) {
    glob_t glob_result;
    std::string dir_path = path + "/*";
    glob(dir_path.c_str(), GLOB_TILDE, nullptr, &glob_result);
    return glob_result;
}

std::string GetDateTimeString(time_t rawtime) {
    struct tm* timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeinfo);
    return {buffer};
}

std::vector<FileData> GetFilesData(const std::string& path) {
    std::vector<FileData> collection;

    glob_t glob_result = get_glob_result(path);
    for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
        //std::cout << glob_result.gl_pathv[i];
        FileData data;
        data.short_name = GetEndOfPath(glob_result.gl_pathv[i]);
        data.full_name = glob_result.gl_pathv[i];
        //collection.push_back(data);
        struct stat stat_result = {};
        if (stat(data.full_name.c_str(), &stat_result) == 0) {
            time_t mod_time = stat_result.st_mtime;
            data.time_string = GetDateTimeString(mod_time);
        }
        collection.push_back(data);
    }
    globfree(&glob_result);
    return collection;
}

std::string CreatePrefix(const std::string& path, const std::string& host) {
    std::string result = "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\"><title>Index of ";
    result += path;
    result += "</title></head>";
    result += "<body>";
    result += "<h1>Index of " + path + "</h1><hr><pre><a href=\"" + host + "/" + GetFirstPartOfPath(path) +
              "\">../</a>\n";
    return result;
}

std::string CreatePostfix() {
    std::string result = "</pre><hr><div at-magnifier-wrapper=\"\"><div class=\"at-theme-light\"><div class=\"at-base notranslate\" translate=\"no\"><div class=\"Z1-AJ\" style=\"top: 0px; left: 0px;\"></div></div></div></div></body></html>";
    return result;
}

std::string CreateSpaces(int lenth, int max) {
    int iterations = max - lenth;
    if (iterations < 0) {
        return "";
    }

    std::string result;
    while (iterations) {
        result += " ";
        iterations -= 1;
    }
    return result;
}

std::string ListDir(const std::string& path, const std::string& trim_path, const std::string& host) {
    std::string result = CreatePrefix(path, host);

    std::vector<FileData> files = GetFilesData(path);

    for (const auto& file_data : files) {
        std::string substring;
        if (std::filesystem::is_directory(file_data.full_name)) {
            substring = "<a href=\"" + trim_path + file_data.short_name + "\">" + "/</a>" +
                    CreateSpaces(file_data.short_name.length(), 67) + file_data.time_string + "\n";
        }
        else {
            substring = "<a href=\"" + trim_path + "\">" + file_data.short_name + "</a>" +
                    CreateSpaces(file_data.short_name.length(), 68) + file_data.time_string + "\n";
        }
        result += substring;
    }

    result += CreatePostfix();
    return result;
}

}

std::string AutoindexHandler::Handle(const std::string& path, const std::string& trim_path, const std::shared_ptr<Request>& request) {
    if (!IsDirectory(path)) {
        throw NotFound("File not found", request->server_config);
    }

    std::string host = request->server_config->host + ":" + std::to_string(request->server_config->port);
    return ListDir(path, trim_path, host);
}

}