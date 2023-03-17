#include <glob.h>
#include <vector>
#include <sys/stat.h>
#include <filesystem>

#include "AutoindexHandler.h"

namespace Http {

namespace {
struct FileData {
    std::string short_name;
    std::string full_name;
    std::string time_string;
    std::string size;
};


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

std::vector<FileData> GetFilesData(const std::string& path, int& max_len) {
    std::vector<FileData> collection;

    max_len = 0;
    glob_t glob_result = get_glob_result(path);
    for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
        FileData data;
        data.short_name = GetEndOfPath(glob_result.gl_pathv[i]);
        data.full_name = glob_result.gl_pathv[i];
        max_len = std::max(static_cast<int>(data.short_name.size()), max_len);
        struct stat stat_result = {};
        if (stat(data.full_name.c_str(), &stat_result) == 0) {
            time_t mod_time = stat_result.st_mtime;
            data.time_string = GetDateTimeString(mod_time);
            data.size = ToReadableSize(stat_result.st_size);
        }
        collection.push_back(data);
    }
    globfree(&glob_result);
    return collection;
}

std::string CreatePrefix(const std::shared_ptr<Location>& location, const std::string& path_after_matching) {
    std::string result = "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\"><title>Index of ";
    result += UnitePaths(location->location, path_after_matching);
    result += "</title></head>";
    result += "<body>";
    result += "<h1>Index of " + UnitePaths(location->location, path_after_matching) + "</h1>";
    result += "<hr><pre>";
    if (!path_after_matching.empty() && path_after_matching != "/") {
        result +=
                "<a href=\"" + UnitePaths(location->location, GetFirstPartOfPath(path_after_matching)) + "\">../</a>\n";
    }
    return result;
}

std::string CreatePostfix() {
    std::string result = "</pre><hr><div at-magnifier-wrapper=\"\"><div class=\"at-theme-light\"><div class=\"at-base notranslate\" translate=\"no\"><div class=\"Z1-AJ\" style=\"top: 0px; left: 0px;\"></div></div></div></div></body></html>";
    return result;
}

std::string CreateSpaces(int length, int max) {
    int iterations = max - length;
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

std::string
ListDir(const std::shared_ptr<Location>& location, const std::string& path, const std::string& path_after_matching) {
    std::string result = CreatePrefix(location, path_after_matching);

    int max_len = 0;
    std::vector<FileData> files = GetFilesData(path, max_len);

    for (const auto& file_data: files) {
        if (std::filesystem::is_directory(file_data.full_name)) {
            result += "<a href=\"" +
                      UnitePaths(UnitePaths(location->location, path_after_matching), file_data.short_name) + "\">" +
                      file_data.short_name + "</a>";
        }
        else {
            result += "<a href=\"" +
                      UnitePaths(UnitePaths(location->location, path_after_matching), file_data.short_name) + "\">" +
                      file_data.short_name + "</a>";
        }
        result += CreateSpaces(static_cast<int>(file_data.short_name.size()), max_len + 20) +
                  file_data.time_string + CreateSpaces(max_len + 20, max_len + 30) + file_data.size + "\n";
    }

    result += CreatePostfix();
    return result;
}

}

std::string AutoindexHandler::Handle(const std::shared_ptr<Location>& location, const std::string& full_path,
                                     const std::string& path_after_matching, const std::shared_ptr<Request>& request) {
    if (!IsDirectory(full_path)) {
        throw NotFound("File not found", request->server_config);
    }

    return ListDir(location, full_path, path_after_matching);
}

}