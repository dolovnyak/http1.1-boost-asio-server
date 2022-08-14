#include "HeaderValue.h"
#include "logging.h"

#include <vector>
#include <sstream>

namespace {
    std::vector<std::string> SplitString(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
}

HeaderValue::HeaderValue(const std::string& value) {
    _value = value;
}

int HeaderValue::AsInt() const {
    try {
        return std::stoi(_value);
    } catch (const std::exception& e) {
        throw std::runtime_error("HeaderValue::AsInt: " + std::string(e.what()));
    }
}

int HeaderValue::AsPositiveInt() const {
    int result = AsInt();
    if (result < 0) {
        throw std::runtime_error("HeaderValue::PositiveInt: value is negative");
    }
    return result;
}

std::string HeaderValue::AsString() const {
    auto tokens = SplitString(_value, ' ');
    if (tokens.size() != 1) {
        throw std::runtime_error("HeaderValue::AsString: value has more than one token");
    }
    return tokens[0];
}

std::pair<std::string, std::string> HeaderValue::AsTwoStrings() const {
    auto tokens = SplitString(_value, ' ');

    if (tokens.size() != 2) {
        throw std::runtime_error("HeaderValue::AsTwoStrings: invalid value: " + _value);
    }

    return {tokens[0], tokens[1]};
}

const std::string& HeaderValue::GetRaw() const {
    return _value;
}
