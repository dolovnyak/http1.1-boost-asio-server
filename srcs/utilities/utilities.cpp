#include "utilities.h"

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <ctime>
#include <cctype>

#define READ_BUFFER_SIZE 4096

std::string StripString(const std::string& str) {
    std::string::const_iterator start_it = str.cbegin();
    std::string::const_reverse_iterator end_it = str.crbegin();

    while (std::isspace(*start_it) && start_it != str.cend())
        ++start_it;
    if (start_it == str.cend())
        return "";

    while (std::isspace(*end_it) && end_it != str.crend())
        ++end_it;

    if (start_it == str.cbegin() && end_it == str.crbegin())
        return str;

    return {start_it, end_it.base()};
}

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

size_t FindInRange(const std::string& str, const std::string& substr, size_t start, size_t end) {
    if (str.empty() || substr.empty() || start >= str.size() || start > end) {
        return std::string::npos;
    }

    size_t pos = str.find(substr, start);
    if (pos == std::string::npos || pos + substr.size() - 1 > end) {
        return std::string::npos;
    }
    return pos;
}

std::string ToLower(const std::string& str) {
    std::string result;
    std::transform(str.begin(), str.end(), std::back_inserter(result), [](unsigned char c){ return std::tolower(c); });
    return result;
}

std::string ToUpper(const std::string& str) {
    std::string result;
    std::transform(str.begin(), str.end(), std::back_inserter(result), [](unsigned char c){ return std::toupper(c); });
    return result;
}

std::string GetCurrentDateTimeString() {
    time_t rawtime;
    time(&rawtime);
    struct tm* timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeinfo);
    return {buffer};
}

std::string UnitePaths(const std::string& path1, const std::string& path2) {
    if (path1.back() == '/' && path2.front() == '/') {
        return path1.substr(0, path1.size() - 1) + path2;
    }
    else if (path1.back() != '/' && path2.front() != '/') {
        return path1 + "/" + path2;
    }
    return path1 + path2;
}

int ParseInt(const std::string& value, int base) {
    return std::stoi(value, nullptr, base);
}

int ParsePositiveInt(const std::string& value, int base) {
    int num_value = ParseInt(value, base);
    if (num_value < 0) {
        throw std::runtime_error("ParsePositiveInt: " + value);
    }
    return num_value;
}

int GetIntFromDigit(char digit) {
    if (digit >= '0' && digit <= '9') {
        return static_cast<int>(digit - '0');
    }
    throw std::runtime_error("GetIntFromDigit: " + std::string(1, digit));
}

bool IsTchar(char c) {
    return isalnum(c) || c == '!' || c == '#' || c == '$' || c == '%' || c == '&' || c == '\'' || c == '*' ||
           c == '+' || c == '-' || c == '.' || c == '^' || c == '_' || c == '`' || c == '|' || c == '~';
}

bool IsTcharString(const std::string& str) {
    for (char c : str) {
        if (!IsTchar(c)) {
            return false;
        }
    }

    return true;
}

bool IsUnreserved(char c) {
    return isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~';
}

bool IsSubDelimiter(char c) {
    return c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' || c == '*' || c == '+' ||
           c == ',' || c == ';' || c == '=';
}

bool IsHexDigit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool IsHexDigitString(const std::string& str) {
    for (char c : str) {
        if (!IsHexDigit(c)) {
            return false;
        }
    }

    return true;
}

// unreserved / pct-encoded / sub-delims / ":" / "@"
bool IsPcharString(const std::string& str) {

    for (size_t start = 0; start < str.size();) {

        if (str.size() - start >= 3 && str[start] == '%' && IsHexDigit(str[start + 1]) && IsHexDigit(str[start + 2])) {
            start += 3;
            continue;
        }

        if (!(IsUnreserved(str[start]) || IsSubDelimiter(str[start]) || str[start] == ':' || str[start] == '@')) {
            return false;
        }
        ++start;
    }
    return true;
}

bool IsAbsolutePath(const std::string& str) {
    if (str.empty() || str[0] != '/') {
        return false;
    }

    size_t prev_slash_pos = 0;
    size_t i = str.find('/', 1);
    for (; i != std::string::npos; i = str.find('/', i + 1)) {
        size_t distance = i - prev_slash_pos;

        if (distance <= 1) {
            return false;
        }

        if (!IsPcharString(str.substr(prev_slash_pos + 1, distance - 1))) {
            return false;
        }
        prev_slash_pos = i;
    }
    if (prev_slash_pos + 1 < str.size() && !IsPcharString(str.substr(prev_slash_pos + 1))) {
        return false;
    }

    return true;
}

bool IsQueryString(const std::string& str) {
    /// like pchar but with "/" and "?"
    for (size_t start = 0; start < str.size();) {

        if (str.size() - start >= 3 && str[start] == '%' && IsHexDigit(str[start + 1]) && IsHexDigit(str[start + 2])) {
            start += 3;
            continue;
        }

        if (!IsUnreserved(str[start]) && !IsSubDelimiter(str[start]) &&
            str[start] != ':' && str[start] != '@' && str[start] != '/' && str[start] != '?') {
            return false;
        }
        ++start;
    }
    return true;
}

bool IsObsText(char c) {
    return static_cast<unsigned char>(c) >= 0x80;
}

bool IsFieldContent(const std::string& str) {
    for (char c : str) {
        if (!isprint(c) && !IsObsText(c) && c != '\t') {
            return false;
        }
    }
    return true;
}

bool IsPositiveNumberString(const std::string& str) {
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool IsRegName(const std::string& str) {
    for (size_t start = 0; start < str.size();) {

        if (str.size() - start >= 3 && str[start] == '%' && IsHexDigit(str[start + 1]) && IsHexDigit(str[start + 2])) {
            start += 3;
            continue;
        }

        if (!IsUnreserved(str[start]) && !IsSubDelimiter(str[start])) {
            return false;
        }
        ++start;
    }
    return true;
}

bool IsIpv4(const std::string& str) {
    size_t prev_dot_pos = 0;
    size_t i = str.find('.', 0);
    for (size_t dots = 0; dots < 3; ++dots) {
        if (i == std::string::npos) {
            return false;
        }
        size_t distance = i - prev_dot_pos;
        if (distance == 0 || distance > 3) {
            return false;
        }
        if (!IsPositiveNumberString(str.substr(prev_dot_pos, distance))) {
            return false;
        }
        int num = ParseInt(str.substr(prev_dot_pos, distance), 10);
        if (num < 0 || num > 255) {
            return false;
        }
        prev_dot_pos = i + 1;
        i = str.find('.', prev_dot_pos);
    }
    size_t distance = str.size() - prev_dot_pos;
    if (distance == 0 || distance > 3) {
        return false;
    }
    if (!IsPositiveNumberString(str.substr(prev_dot_pos, distance))) {
        return false;
    }
    int num = ParseInt(str.substr(prev_dot_pos, distance), 10);
    if (num < 0 || num > 255) {
        return false;
    }
    return true;
}

bool SetSocketNonBlocking(int fd) {
    int option_value = 1;
    if (ioctl(fd, FIONBIO, (char*)&option_value) < 0) {
        return false;
    }
    return true;
}

bool ReadFile(const std::string& path, std::string& result) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        return false;
    }
    while (true) {
        char buff[READ_BUFFER_SIZE];

        ssize_t read_bytes = read(fd, buff, sizeof(buff));
        if (read_bytes < 0) {
            close(fd);
            return false;
        }
        if (read_bytes == 0) {
            break;
        }
        result.append(buff, read_bytes);
    }
    close(fd);
    return true;
}

bool IsFile(const std::string& path) {
    struct stat path_stat{};
    if (stat(path.c_str(), &path_stat) != 0) {
        return false;
    }
    return S_ISREG(path_stat.st_mode);
}

bool IsExecutableFile(const std::string& path) {
    struct stat path_stat{};
    if (stat(path.c_str(), &path_stat) != 0) {
        return false;
    }
    return (S_IXUSR & path_stat.st_mode) || (S_IEXEC & path_stat.st_mode);
}

bool IsDirectory(const std::string& path) {
    struct stat path_stat{};
    if (stat(path.c_str(), &path_stat) != 0) {
        return false;
    }
    return S_ISDIR(path_stat.st_mode);
}
