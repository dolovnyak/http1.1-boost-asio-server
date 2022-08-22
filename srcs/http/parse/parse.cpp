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

//int BaseHeaderValue::AsInt() const {
//    try {
//        return std::stoi(_value);
//    } catch (const std::exception& e) {
//        throw std::runtime_error("HeaderValue::AsInt: " + std::string(e.what()));
//    }
//}
//
//int BaseHeaderValue::AsPositiveInt() const {
//    int result = AsInt();
//    if (result < 0) {
//        throw std::runtime_error("HeaderValue::PositiveInt: value is negative");
//    }
//    return result;
//}

//std::string BaseHeaderValue::AsString() const {
//    auto tokens = SplitString(_value, ' ');
//    if (tokens.size() != 1) {
//        throw std::runtime_error("HeaderValue::AsString: value has more than one token");
//    }
//    return tokens[0];
//}

//std::pair<std::string, std::string> BaseHeaderValue::AsTwoStrings() const {
//    auto tokens = SplitString(_value, ' ');
//
//    if (tokens.size() != 2) {
//        throw std::runtime_error("HeaderValue::AsTwoStrings: invalid value: " + _value);
//    }
//
//    return {tokens[0], tokens[1]};
//}
