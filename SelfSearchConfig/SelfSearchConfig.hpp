#ifndef SELFSEARCHCONFIG_HPP
#define SELFSEARCHCONFIG_HPP

#include <string>
#include <nlohmann/json.hpp>

class SelfSearchConfig {
public:
    std::string selfIp;         // The IP to configure on the device
    std::string ipToPing;       // The IP to ping for connectivity check
    std::string destIpAddress;
    std::string defaultGateway;
    std::string subnetMask;
    std::string remoteIp;
    std::string remoteIpDestination;
    std::string role;           // Role as a string (e.g., "source1")

    // JSON deserialization for SelfSearchConfig
    friend void from_json(const nlohmann::json& j, SelfSearchConfig& config);
};

#endif // SELFSEARCHCONFIG_HPP
