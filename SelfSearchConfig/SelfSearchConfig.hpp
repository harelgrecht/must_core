#pragma once

#include <string>
#include "../third_party/nlohmann/json.hpp"

class SelfSearchConfig {
public:
    std::string selfIp_;         // The IP to configure on the device
    std::string ipToPing_;       // The IP to ping for connectivity check
    std::string destIpAddress_;
    std::string defaultGateway_;
    std::string subnetMask_;
    std::string remoteIp_;
    std::string remoteIpDestination_;
    std::string role_;           // Role as a string (e.g., "source1")

    // JSON deserialization for SelfSearchConfig
    friend void from_json(const nlohmann::json& j, SelfSearchConfig& config);
};

