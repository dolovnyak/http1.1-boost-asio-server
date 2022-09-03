#pragma once

#include <string>
#include <vector>

#ifdef _STANDARD98
#define OVERRIDE
#else
#define OVERRIDE override
#endif

std::string StripString(const std::string& str);

std::vector<std::string> SplitString(const std::string& str, const std::string& delimiters);

size_t FindInRange(const std::string& str, const std::string& substr, size_t start, size_t end);

std::string ToLower(const std::string& str);

std::string GetCurrentDateTime();

int ParseInt(const std::string& value, int base = 10);

int ParsePositiveInt(const std::string& value, int base = 10);

int GetIntFromDigit(char digit);

bool IsTchar(char c);

bool IsTcharString(const std::string& str);

bool IsUnreserved(char c);

bool IsSubDelimiter(char c);

bool IsHexDigit(char c);

bool IsPcharString(const std::string& str);

bool IsAbsolutePath(const std::string& str);

bool IsQueryString(const std::string& str);

bool IsFieldContent(const std::string& str);

bool IsPositiveNumberString(const std::string& str);

bool SetSocketNonBlocking(int socket);
