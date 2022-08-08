#pragma once

#include <string>
#include <iostream>

#define TEXT_RED std::string("\e[91m")
#define TEXT_GREEN std::string("\e[92m")
#define TEXT_YELLOW std::string("\e[93m")
#define TEXT_MAGENTA std::string("\e[95m")
#define TEXT_CYAN std::string("\e[96m")
#define TEXT_BLACK std::string("\e[30m")
#define TEXT_DEFAULT std::string("\e[39m")
#define BG_RED std::string("\e[41m")
#define BG_YELLOW std::string("\e[43m")
#define BG_GREEN std::string("\e[42m")
#define BG_GRAY std::string("\e[47m")
#define BG_CYAN std::string("\e[46m")
#define BG_DEFAULT std::string("\e[49m")

namespace {
    template<typename T>
    void Print(const std::string& text_color, const std::string& bg_color, const T& val) {
        std::cout << text_color + bg_color;
        std::cout << val;
        std::cout << TEXT_DEFAULT + BG_DEFAULT;
    };

    template<typename T>
    void PrintWithErrno(const std::string& text_color, const std::string& bg_color, const T& val) {
        std::cout << text_color + bg_color;
        std::cout << val << " (errno: " << strerror(errno) << ")" << std::endl;
        std::cout << TEXT_DEFAULT + BG_DEFAULT;
    };
}

class Log {
public:
    template<typename T>
    static void Info(T t) {
        Print(TEXT_DEFAULT, BG_DEFAULT, t);
        std::cout << std::endl;
    };

    template<typename T, typename... Args>
    static void Info(T t, Args... args) {
        Print(TEXT_DEFAULT, BG_DEFAULT, t);
        Info(args...);
    };

    template<typename T>
    static void Error(T t) {
        Print(TEXT_RED, BG_DEFAULT, t);
        std::cout << std::endl;
    };

    template<typename T, typename... Args>
    static void Error(T t, Args... args) {
        Print(TEXT_RED, BG_DEFAULT, t);
        Error(args...);
    };

    template<typename T>
    static void Perror(T t) {
        PrintWithErrno(TEXT_RED, BG_DEFAULT, t);
        std::cout << std::endl;
    };

    template<typename T, typename... Args>
    static void Perror(T t, Args... args) {
        PrintWithErrno(TEXT_RED, BG_DEFAULT, t);
        Error(args...);
    };

    template<typename T>
    static void Warning(T t) {
        Print(TEXT_YELLOW, BG_DEFAULT, t);
        std::cout << std::endl;
    };

    template<typename T, typename... Args>
    static void Warning(T t, Args... args) {
        Print(TEXT_YELLOW, BG_DEFAULT, t);
        Warning(args...);
    };

    template<typename T>
    static void Important(T t) {
        Print(TEXT_CYAN, BG_DEFAULT, t);
        std::cout << std::endl;
    };

    template<typename T, typename... Args>
    static void Important(T t, Args... args) {
        Print(TEXT_CYAN, BG_DEFAULT, t);
        Important(args...);
    };

    template<typename T>
    static void Success(T t) {
        Print(TEXT_GREEN, BG_DEFAULT, t);
        std::cout << std::endl;
    };

    template<typename T, typename... Args>
    static void Success(T t, Args... args) {
        Print(TEXT_GREEN, BG_DEFAULT, t);
        Success(args...);
    };

    template<typename T>
    static void ErrorBg(T t) {
        Print(TEXT_BLACK, BG_RED, t);
        std::cout << std::endl;
    };

    template<typename T, typename... Args>
    static void ErrorBg(T t, Args... args) {
        Print(TEXT_BLACK, BG_RED, t);
        ErrorBg(args...);
    };

    template<typename T>
    static void PerrorBg(T t) {
        PrintWithErrno(TEXT_BLACK, BG_RED, t);
        std::cout << std::endl;
    };

    template<typename T, typename... Args>
    static void PerrorBg(T t, Args... args) {
        PrintWithErrno(TEXT_BLACK, BG_RED, t);
        Error(args...);
    };

    template<typename T>
    static void WarningBg(T t) {
        Print(TEXT_BLACK, BG_YELLOW, t);
        std::cout << std::endl;
    };

    template<typename T, typename... Args>
    static void WarningBg(T t, Args... args) {
        Print(TEXT_BLACK, BG_YELLOW, t);
        WarningBg(args...);
    };

    template<typename T>
    static void ImportantBg(T t) {
        Print(TEXT_BLACK, BG_CYAN, t);
        std::cout << std::endl;
    };

    template<typename T, typename... Args>
    static void ImportantBg(T t, Args... args) {
        Print(TEXT_BLACK, BG_CYAN, t);
        ImportantBg(args...);
    };

    template<typename T>
    static void SuccessBg(T t) {
        Print(TEXT_BLACK, BG_GREEN, t);
        std::cout << std::endl;
    };

    template<typename T, typename... Args>
    static void SuccessBg(T t, Args... args) {
        Print(TEXT_BLACK, BG_GREEN, t);
        SuccessBg(args...);
    };

    template<typename T>
    static void InfoBg(T t) {
        Print(TEXT_BLACK, BG_GRAY, t);
        std::cout << std::endl;
    };

    template<typename T, typename... Args>
    static void InfoBg(T t, Args... args) {
        Print(TEXT_BLACK, BG_GRAY, t);
        InfoBg(args...);
    };
};

#ifndef _DEBUG
#define LOG_SUCCESS(...)
#define LOG_SUCCESS_BG(...)
#define LOG_WARNING(...)
#define LOG_WARNING_BG(...)
#define LOG_IMPORTANT(...)
#define LOG_IMPORTANT_BG(...)
#define LOG_INFO(...)
#define LOG_INFO_BG(...)
#define LOG_ERROR(...)          Log::Error(__VA_ARGS__)
#define LOG_ERROR_BG(...)       Log::ErrorBg(__VA_ARGS__)
#define LOG_PERROR(...)         Log::Perror(__VA_ARGS__)
#define LOG_PERROR_BG(...)      Log::PerrorBg(__VA_ARGS__)
#else
#define LOG_SUCCESS(...)        Log::Success(__VA_ARGS__)
#define LOG_SUCCESS_BG(...)     Log::SuccessBg(__VA_ARGS__)
#define LOG_WARNING(...)        Log::Warning(__VA_ARGS__)
#define LOG_WARNING_BG(...)     Log::WarningBg(__VA_ARGS__)
#define LOG_IMPORTANT(...)      Log::Important(__VA_ARGS__)
#define LOG_IMPORTANT_BG(...)   Log::ImportantBg(__VA_ARGS__)
#define LOG_INFO(...)       	Log::Info(__VA_ARGS__)
#define LOG_INFO_BG(...)       	Log::InfoBg(__VA_ARGS__)
#define LOG_ERROR(...)          Log::Error(__VA_ARGS__)
#define LOG_ERROR_BG(...)       Log::ErrorBg(__VA_ARGS__)
#define LOG_PERROR(...)         Log::Perror(__VA_ARGS__)
#define LOG_PERROR_BG(...)      Log::PerrorBg(__VA_ARGS__)
#endif