#pragma once

#include <string>
#include <optional>

class BaseHeaderValue {

public:
    BaseHeaderValue(const std::string& value);

    int AsInt() const;

    int AsPositiveInt() const;

    std::string AsString() const;

    std::pair<std::string, std::string> AsTwoStrings() const;

    const std::string& GetRaw() const;

private:
    std::string _value;
};


///    key    key    :  asdasdasdf    sdfasdf

