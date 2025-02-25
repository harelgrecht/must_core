#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>
#include <sys/sysinfo.h>
#include <sstream>

enum class LogLevel 
{ 
    INFO = 0, 
    ERROR 
};

class Logger {
public:
    explicit Logger(const std::string& logPath);
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void logWrite(LogLevel level, const std::string& message);

private:
    unsigned long getSystemUptime() const;
    
    std::ofstream logFile_;
    std::string logPath_;
    mutable std::mutex logMutex_;

    static constexpr const char* levelStr[] = {"INFO", "ERROR"};
};