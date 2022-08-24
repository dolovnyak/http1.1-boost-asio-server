#include "parse.h"

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
