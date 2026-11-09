#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <mutex>

namespace Jonaki {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    static void init(const std::string& logFilePath = "");
    static void log(LogLevel level, std::string_view message);
    static void debug(std::string_view message);
    static void info(std::string_view message);
    static void warning(std::string_view message);
    static void error(std::string_view message);

private:
    static std::mutex s_mutex;
    static std::string s_logFilePath;
};

} // namespace Jonaki
