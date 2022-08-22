#pragma once

#include <string>
#include <vector>

std::string StripString(const std::string& str);

std::vector<std::string> SplitString(const std::string& str, const std::string& delimiters);

size_t FindInRange(const std::string& str, const std::string& substr, size_t start, size_t end);

std::string ToLower(const std::string& str);