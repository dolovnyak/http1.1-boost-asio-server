#pragma once

#include <string>
#include <vector>

std::string StripString(const std::string& str);

std::vector<std::string> SplitString(const std::string& str, const std::string& delimiters);

std::string ToLower(const std::string& str);