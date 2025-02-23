#include "SelfSearchConfig.hpp"

void from_json(const nlohmann::json& j, SelfSearchConfig& config) {
    config.selfIp           = j.value("selfIp", "");
    config.ipToPing         = j.value("ipToPing", "");
    config.destIpAddress    = j.value("DestIpAddress", "");
    config.defaultGateway   = j.value("DefaultGateway", "");
    config.subnetMask       = j.value("SubnetMask", "255.255.255.0");
    config.remoteIp         = j.value("RemoteIp", "");
    config.remoteIpDestination = j.value("RemoteIpDestination", "");
    config.role             = j.value("role", "source1");
}
