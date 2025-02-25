#include "NetworkManager.hpp"
#include <iostream>

int main() {
    try {
        NetworkManager networkManager;
        // Parse the user settings (user_setting.json) and, if needed, the self-search config (self_search_ips.json)
        networkManager.parseSettings("user_setting.json", "self_search_ips.json");
        // Depending on the enableSelfSearch flag, the system either applies the user settings or runs self-search mode.
        networkManager.applySettings();
    } catch (const std::exception& ex) {
        std::cerr << "[Error] " << ex.what() << std::endl;
    }
    return 0;
}
