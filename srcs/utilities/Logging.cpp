#include "Logging.h"
    void Log::TimerStart(const std::string& name) {
        _timer = std::chrono::system_clock::now();
        _timer_name = name;
    };

    void Log::LogTime() {
        auto end = std::chrono::system_clock::now();
        auto seconds = end - _timer;
        Log::Info("Time elapsed from ", _timer_name, " start: ", std::chrono::duration_cast<std::chrono::microseconds>(seconds).count(), "us ");
    };

std::chrono::time_point<std::chrono::system_clock> Log::_timer;
std::string Log::_timer_name;
