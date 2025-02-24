// gpioHandler.cpp
#include "gpioHandler.hpp"

nlohmann::json gpioHandler::config = nlohmann::json();
std::string gpioHandler::configFilePath = "";

void gpioHandler::setConfigPath(const std::string& configPath) {
    if (configFilePath.empty()) {
        configFilePath = configPath;
        loadConfig();
    } else {
        std::cerr << "Configuration path is already set." << std::endl;
    }
}

void gpioHandler::loadConfig() {
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open config file: " << configFilePath << std::endl;
        return;
    }
    try {
        config = json::parse(configFile);
    } catch (json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return;
    }
}

gpioHandler::gpioHandler(int ledPin, const std::string& direction) : pinNumber(ledPin) {
    // Export GPIO pin
    if (!writeToFile(config["GPIO_EXPORT_PATH"], std::to_string(pinNumber))) {
        std::cerr << "Error: Failed to export GPIO pin " << pinNumber << std::endl;
    }

    // Set GPIO direction
    if (!writeToFile(formatPath(config["GPIO_DIRECTION_PATH"].get<std::string>().c_str()), direction)) {
        std::cerr << "Error: Failed to set direction for GPIO pin " << pinNumber << std::endl;
    }
}

gpioHandler::~gpioHandler() {
    if (!writeToFile(config["GPIO_UNEXPORT_PATH"], std::to_string(pinNumber))) {
        std::cerr << "Warning: Failed to unexport GPIO pin " << pinNumber << std::endl;
    }
}

bool gpioHandler::setHigh() {
    return writeToFile(formatPath(config["GPIO_VALUE_PATH"].get<std::string>().c_str()), std::to_string(config["GPIO_ON"]));
}

bool gpioHandler::setLow() {
    return writeToFile(formatPath(config["GPIO_VALUE_PATH"].get<std::string>().c_str()), std::to_string(config["GPIO_OFF"]));
}

bool gpioHandler::blinkLed() {
    std::string valuePath = formatPath(config["GPIO_VALUE_PATH"].get<std::string>().c_str());
    if (!writeToFile(valuePath, std::to_string(config["GPIO_ON"]))) {
        return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return writeToFile(valuePath, std::to_string(config["GPIO_OFF"]));
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
