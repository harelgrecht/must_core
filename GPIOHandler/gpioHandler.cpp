#include "gpioHandler.hpp"
#include <thread>
#include <chrono>

gpioHandler::gpioHandler(int ledPin, const std::string& direction) : pinNumber(ledPin) {
    if (!writeToFile(GPIO_EXPORT_PATH, std::to_string(pinNumber))) {
        std::cerr << "Error: Failed to export GPIO pin " << pinNumber << std::endl;
    }

    if (!writeToFile(formatPath(GPIO_DIRECTION_PATH), direction)) {
        std::cerr << "Error: Failed to set direction for GPIO pin " << pinNumber << std::endl;
    }
}


gpioHandler::~gpioHandler() {
    if (!writeToFile(GPIO_UNEXPORT_PATH, std::to_string(pinNumber))) {
        std::cerr << "Warning: Failed to unexport GPIO pin " << pinNumber << std::endl;
    }
}

bool gpioHandler::setHigh() {
    return writeToFile(formatPath(GPIO_VALUE_PATH), std::to_string(GPIO_ON));
}

bool gpioHandler::setLow() {
    return writeToFile(formatPath(GPIO_VALUE_PATH), std::to_string(GPIO_OFF));
}

bool gpioHandler::blinkLed() {
    std::string valuePath = formatPath(GPIO_VALUE_PATH);
    if (!writeToFile(valuePath, std::to_string(GPIO_ON))) {
        return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return writeToFile(valuePath, std::to_string(GPIO_OFF));
}

std::string gpioHandler::formatPath(const char* pathTemplate) const {
    char formattedPath[100];
    snprintf(formattedPath, sizeof(formattedPath), pathTemplate, pinNumber);
    return std::string(formattedPath);
}

bool gpioHandler::writeToFile(const std::string& filePath, const std::string& data) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open: " << filePath << std::endl;
        return false;
    }
    file << data;
    if (file.fail()) {
        std::cerr << "Failed to write to: " << filePath << std::endl;
        return false;
    }
    return true;
}
