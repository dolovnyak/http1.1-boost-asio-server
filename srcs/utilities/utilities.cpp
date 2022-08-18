#include "utilities.h"

std::string StripString(const std::string& str) {
    std::string::const_iterator start_it = str.cbegin();
    std::string::const_reverse_iterator end_it = str.crbegin();

    while (std::isspace(*start_it))
        ++start_it;
    while (std::isspace(*end_it))
        ++end_it;
    return std::string(start_it, end_it.base());
}

std::vector<std::string> SplitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;

    while (true) {
        size_t token_start = str.find_first_not_of(delimiter, pos);
        if (token_start == std::string::npos) {
            return tokens;
        }

        size_t token_end = str.find_first_of(delimiter, token_start);
        if (token_end == std::string::npos) {
            tokens.push_back(str.substr(token_start));
            return tokens;
        }

        tokens.push_back(str.substr(token_start, token_end - token_start));
        pos = token_end + 1;
    }
}

bool Contains(const std::string& str, char c) {
    return str.find(c) != std::string::npos;
}

