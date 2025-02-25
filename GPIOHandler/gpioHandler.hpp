// gpioHandler.hpp
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <thread>
#include <chrono>
#include "../nlohmann/json.hpp"

namespace json = nlohmann;

class gpioHandler {
public:
    gpioHandler(int ledPin, const std::string& direction);
    ~gpioHandler();

    bool setHigh();
    bool setLow();
    bool blinkLed();

    static void setConfigPath(const std::string& configPath);

private:
    bool writeToFile(const std::string& filePath, const std::string& data);
    std::string formatPath(const char* pathTemplate) const;

    int pinNumber;
    static json config; // Static JSON object to store configuration
    static std::string configFilePath; // Static string to store the config file path

    static void loadConfig();
};
