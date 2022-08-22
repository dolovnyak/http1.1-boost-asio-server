#include "utilities.h"

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

    return std::string(start_it, end_it.base());
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
    size_t pos = str.find(substr, start);
    if (pos == std::string::npos || pos >= end) {
        return std::string::npos;
    }
    return pos;
}

std::string ToLower(const std::string& str) {
    std::string result;
    std::transform(str.begin(), str.end(), std::back_inserter(result), std::tolower);
    return result;
}
