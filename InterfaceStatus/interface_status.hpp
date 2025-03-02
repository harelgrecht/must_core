#pragma once

#include <iostream>
#include <string> 
#include <array>
#include <thread>
#include <atomic>
#include <sstream>
#include <fstream>
#include <chrono>
#include "../gpioHandler/gpioHandler.hpp"
#include "../logHandler/logger.hpp"

struct InterfaceMapEntry {
    std::string name;
    gpioHandler greenLed;
    gpioHandler yellowLed;

    InterfaceMapEntry(const std::string& interface, int greenPin, int yellowPin) 
        : name(interface), greenLed(greenPin, "out"), yellowLed(yellowPin, "out") { }
};

class InterfaceStatus {
    public:
        InterfaceStatus();
        ~InterfaceStatus();
        void StartEthStatus();
        void StartUpLeds();


    private:
        static constexpr int ETH_COUNT = 4;
        std::array<InterfaceMapEntry, ETH_COUNT> interfaceMap;
        std::array<size_t, ETH_COUNT> previousRxBytes;

        std::thread statusThread;
        std::atomic<bool> runThread;
        

        size_t getRxBytes(const std::string& interfaceName);
        size_t getTxBytes(const std::string& interfaceName);
        void updateEthStatus();
};