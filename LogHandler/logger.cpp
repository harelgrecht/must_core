#include "logger.hpp"

Logger::Logger(const std::string& logPath) : logPath_(logPath), logFile_(logPath, std::ios::app) {
    if (!logFile_.is_open()) {
        throw std::runtime_error("Failed to open log file: " + logPath_);
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

unsigned long Logger::getSystemUptime() const {
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        std::cerr << "[Logger] sysinfo call failed" << std::endl;
        return 0;
    }
    return info.uptime; 
}

void Logger::logWrite(LogLevel level, const std::string& message) {
    static constexpr const char* levelStr[] = {"INFO", "ERROR"};
    const auto uptime = getSystemUptime();
    #ifdef PRINT_TO_SCREEN
        std::cout << "[" << uptime << "] [" 
                  << levelStr[static_cast<int>(level)] << "] " 
                  << message << std::endl;
    #else
        std::lock_guard<std::mutex> lock(logMutex_); 
        if (logFile_.is_open()) {
            logFile << "[" << uptime << "] [" 
                    << levelStr[static_cast<int>(level)] << "] " 
                    << message << std::endl;
        } else {
            std::cerr << "[Logger] Error writing to log file: " << logPath_ << std::endl;
        }
    #endif
}
