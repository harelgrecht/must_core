#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>

constexpr int GPIO_ON = 1;
constexpr int GPIO_OFF = 0;

constexpr char GPIO_EXPORT_PATH[] = "/sys/class/gpio/export";
constexpr char GPIO_UNEXPORT_PATH[] = "/sys/class/gpio/unexport";
constexpr char GPIO_DIRECTION_PATH[] = "/sys/class/gpio/gpio%d/direction";
constexpr char GPIO_VALUE_PATH[] = "/sys/class/gpio/gpio%d/value";

class gpioHandler {
public:
    gpioHandler(int ledPin, const std::string& direction);
    ~gpioHandler();

    bool setHigh();
    bool setLow();
    bool blinkLed();

private:
    bool writeToFile(const std::string& filePath, const std::string& data);
    std::string formatPath(const char* pathTemplate) const;

    int pinNumber;
};
