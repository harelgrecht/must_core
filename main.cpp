#include "LogHandler/logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void threadFunction(Logger& logger, LogLevel level, const std::string& message) {
    for (int i = 0; i < 5; ++i) {
        logger.logWrite(level, message + " - Iteration: " + std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate some work
    }
}

int main() {
    try {
        Logger logger("example_log.txt");

        // Log messages from the main thread
        logger.logWrite(LogLevel::INFO, "Application started.");
        
        // Create multiple threads to log messages concurrently
        std::thread t1(threadFunction, std::ref(logger), LogLevel::INFO, "Thread 1");
        std::thread t2(threadFunction, std::ref(logger), LogLevel::ERROR, "Thread 2");

        // Join the threads
        t1.join();
        t2.join();

        logger.logWrite(LogLevel::INFO, "Application finished successfully.");

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}