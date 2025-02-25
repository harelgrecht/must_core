#include "NetworkManager.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <thread>
#include <chrono>

NetworkManager::NetworkManager() {
    // Initialize 4 Ethernet devices (eth1, eth2, eth3, eth4)
    ethDevices[0] = EthDevice("eth1");
    ethDevices[1] = EthDevice("eth2");
    ethDevices[2] = EthDevice("eth3");
    ethDevices[3] = EthDevice("eth4");
}

// Parse the user settings file to get the normal EthDevice settings and the flag.
// If self-search is enabled, also load self-search configurations from the separate file.
void NetworkManager::parseSettings(const std::string& userSettingsFile, const std::string& selfSearchFile) {
    std::ifstream userFile(userSettingsFile);
    if (!userFile.is_open()) {
        throw std::runtime_error("Failed to open user settings file: " + userSettingsFile);
    }
    nlohmann::json userConfig;
    userFile >> userConfig;

    // Update each EthDevice with settings from user_setting.json
    for (auto& device : ethDevices) {
        std::string name = device.getName();
        if (userConfig.contains(name)) {
            auto tempDevice = EthDevice(name);
            tempDevice = userConfig.at(name).get<EthDevice>();
            device = tempDevice;
        }
    }

    // Read the enableSelfSearch flag from the user settings.
    enableSelfSearch = userConfig.value("enableSelfSearch", false);

    if (enableSelfSearch) {
        // In self-search mode, load configurations from self_search_ips.json.
        std::ifstream searchFile(selfSearchFile);
        if (!searchFile.is_open()) {
            throw std::runtime_error("Failed to open self search file: " + selfSearchFile);
        }
        nlohmann::json searchConfig;
        searchFile >> searchConfig;
        if (searchConfig.is_array()) {
            selfSearchConfigs = searchConfig.get<std::vector<SelfSearchConfig>>();
        } else {
            throw std::runtime_error("Self search configuration is not an array.");
        }
    }
}

// In normal mode, apply settings for each EthDevice and set system routes.
// In self-search mode, run the self-search routine.
void NetworkManager::applySettings() {
    if (!enableSelfSearch) {
        std::cout << "[NetworkManager] Normal mode: applying user settings.\n";
        for (auto& device : ethDevices) {
            device.applySettings();
        }
        setSystemRoutes();
    } else {
        std::cout << "[NetworkManager] Self search mode enabled. Running self-search routines...\n";
        runSelfSearchInParallel();
    }
}

void NetworkManager::setSystemRoutes() {
    std::cout << "[NetworkManager] Setting system routes (normal mode placeholder).\n";
}

// For each Ethernet device, check cable connectivity and then run self-search configurations.
// If the cable is not detected, retry up to 5 times (over about 1 minute).
void NetworkManager::runSelfSearchForDevice(EthDevice& device, const std::vector<SelfSearchConfig>& configs) {
    const int maxAttempts = 5;
    int attempts = 0;
    while (!device.isCableConnected() && attempts < maxAttempts) {
        std::cout << "[NetworkManager] Cable not connected on " << device.getName()
                  << ", attempt " << (attempts + 1) << " of " << maxAttempts << ". Retrying...\n";
        std::this_thread::sleep_for(std::chrono::seconds(12));
        attempts++;
    }
    if (!device.isCableConnected()) {
        std::cout << "[NetworkManager] Cable still not connected on " << device.getName()
                  << " after " << maxAttempts << " attempts. Skipping configuration.\n";
        return;
    }

    // Cable is connected; iterate over each self-search configuration.
    bool configApplied = false;
    for (const auto& config : configs) {
        device.applySelfSearchConfig(config);
        if (device.pingIP(config.ipToPing)) {
            std::cout << "[NetworkManager] Ping successful for config with ipToPing " << config.ipToPing
                      << " on device " << device.getName() << ". Applying configuration.\n";
            configApplied = true;
            break;
        } else {
            std::cout << "[NetworkManager] Ping failed for config with ipToPing " << config.ipToPing
                      << " on device " << device.getName() << ". Trying next configuration.\n";
        }
    }
    if (!configApplied) {
        std::cout << "[NetworkManager] No valid self-search configuration found for device " << device.getName() << ".\n";
    }
}

// Launch self-search for each Ethernet device in parallel.
void NetworkManager::runSelfSearchInParallel() {
    std::vector<std::thread> threads;
    for (auto& device : ethDevices) {
        threads.emplace_back([this, &device]() {
            runSelfSearchForDevice(device, selfSearchConfigs);
        });
    }
    for (auto& t : threads) {
        t.join();
    }
    std::cout << "[NetworkManager] Self-search tasks completed for all devices.\n";
}
