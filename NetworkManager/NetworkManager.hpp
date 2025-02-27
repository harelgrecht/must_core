#pragma once

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include "..//third_party/nlohmann/json.hpp"
#include "../EthDevice/EthDevice.hpp"
#include "../SelfSearchConfig/SelfSearchConfig.hpp"

class NetworkManager {
public:
    NetworkManager();

    // Parse settings from the user settings file and, if needed, from the self-search file.
    // 'userSettingsFile' is the path to user_setting.json.
    // 'selfSearchFile' is the path to self_search_ips.json.
    void parseSettings(const std::string& userSettingsFile, const std::string& selfSearchFile);

    // Based on the enableSelfSearch flag, either apply the normal settings or run self-search.
    void applySettings();

    static EthDevice getDeviceByRole(EthDevice::Role role);

private:
    static std::array<EthDevice, 4> NetworkManager::ethDevices_;
    bool enableSelfSearch { false };

    // In self-search mode, the configurations loaded from self_search_ips.json
    std::vector<SelfSearchConfig> selfSearchConfigs_;

    // For normal mode: (if needed) set system-wide routes.
    void setSystemRoutes();

    // In self-search mode: run self-search for each device in parallel.
    void runSelfSearchInParallel();

    // For a given device, run the self-search routine using the provided configurations.
    void runSelfSearchForDevice(EthDevice& device, const std::vector<SelfSearchConfig>& configs);
};

