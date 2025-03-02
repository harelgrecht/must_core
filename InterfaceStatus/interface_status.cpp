#include "interface_Status.hpp"

InterfaceStatus::InterfaceStatus() : runThread(false), 
    interfaceMap({
        InterfaceMapEntry("eth0", 10, 11),
        InterfaceMapEntry("eth1", 12, 13),
        InterfaceMapEntry("eth2", 14, 15),
        InterfaceMapEntry("eth3", 16, 17)
    }) {
        previousRxBytes.fill(0);
}

InterfaceStatus::~InterfaceStatus() {
    runThread = false;
    if(statusThread.joinable())
        statusThread.join();
}


void InterfaceStatus::StartEthStatus() {
    runThread = true;
    statusThread = std::thread(&InterfaceStatus::updateEthStatus, this);
}

void InterfaceStatus::updateEthStatus() {
    // call the isCableConnected() from the network manager
    while(runThread) {
        for (size_t i = 0; i < ETH_COUNT; i++) {
            size_t currentRxBytes = getRxBytes(interfaceMap[i].name);
            if(currentRxBytes > previousRxBytes[i]) {
                interfaceMap[i].greenLed.blinkLed();
            }
            previousRxBytes[i] = currentRxBytes;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void InterfaceStatus::StartUpLeds() {
    const int repeatCount = 2;
    for (int cycle = 0; cycle < repeatCount; ++cycle) {
        // Turn on LEDs from left to right: yellow then green for each interface.
        for (auto& entry : interfaceMap) {
            entry.yellowLed.setHigh();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            entry.greenLed.setHigh();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        // Turn off LEDs from right to left: yellow then green for each interface.
        for (auto it = interfaceMap.rbegin(); it != interfaceMap.rend(); ++it) {
            it->yellowLed.setLow();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            it->greenLed.setLow();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
}

size_t InterfaceStatus::getRxBytes(const std::string& interfaceName) {
    std::string path = "/sys/class/net/" + interfaceName + "/statistics/rx_bytes";
    std::ifstream file(path);
    if (!file.is_open()) {
        Logger::getInstance().Log(LogLevel::ERROR, "unalbe to read file: " + path);
        return 0;
    }
    size_t rxBytes = 0;
    file >> rxBytes;
    return rxBytes;
}

size_t InterfaceStatus::getTxBytes(const std::string& interfaceName) {
    std::string path = "/sys/class/net/" + interfaceName + "/statistics/tx_bytes";
    std::ifstream file(path);
    if (!file.is_open()) {
        Logger::getInstance().Log(LogLevel::ERROR, "unalbe to read file: " + path);
        return 0;
    }
    size_t txBytes = 0;
    file >> txBytes;
    return txBytes;
}