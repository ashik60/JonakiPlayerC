#include "Logger.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace Jonaki {

std::mutex Logger::s_mutex;
std::string Logger::s_logFilePath;

void Logger::init(const std::string& logFilePath) {
    std::lock_guard<std::mutex> lock(s_mutex);
    s_logFilePath = logFilePath;
}

void Logger::log(LogLevel level, std::string_view message) {
    std::lock_guard<std::mutex> lock(s_mutex);

    const char* levelStr = "INFO";
    switch (level) {
        case LogLevel::Debug:   levelStr = "DEBUG"; break;
        case LogLevel::Info:    levelStr = "INFO"; break;
        case LogLevel::Warning: levelStr = "WARN"; break;
        case LogLevel::Error:   levelStr = "ERROR"; break;
    }

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm buf{};
#if defined(_WIN32)
    localtime_s(&buf, &in_time_t);
#else
    localtime_r(&in_time_t, &buf);
#endif

    std::ostringstream ss;
    ss << "[" << std::put_time(&buf, "%Y-%m-%d %H:%M:%S") << "] [" << levelStr << "] " << message << "\n";
    std::string formatted = ss.str();

    std::cout << formatted;

    if (!s_logFilePath.empty()) {
        std::ofstream outFile(s_logFilePath, std::ios_base::app);
        if (outFile.is_open()) {
            outFile << formatted;
        }
    }
}

void Logger::debug(std::string_view message) {
    log(LogLevel::Debug, message);
}

void Logger::info(std::string_view message) {
    log(LogLevel::Info, message);
}

void Logger::warning(std::string_view message) {
    log(LogLevel::Warning, message);
}

void Logger::error(std::string_view message) {
    log(LogLevel::Error, message);
}

} // namespace Jonaki
