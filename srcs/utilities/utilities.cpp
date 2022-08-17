#include "utilities.h"

#include <sstream>

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
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
