#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>
#include <sys/sysinfo.h>

/*
 * @brief Log levels for the logger.
 */
enum class LogLevel 
{ 
    INFO = 0, 
    ERROR 
};

class Logger {
    public:
        /*
        * @brief Constructs the logger, opening the file at the given path for appending.
        * @param logPath Path the log file.
        * @throws std::runtime_error if the file cannot be opened.
        */
        explicit Logger(const std::string& logPath);

        ~Logger();

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
 
        void logWrite(LogLevel level, const std::string& message);

    private:
        unsigned long getSystemUptime() const;
        
        std::ofstream logFile_;
        std::string logPath_;
        std::mutex logMutex_;

};

