#include "SelfSearchConfig.hpp"

void from_json(const nlohmann::json& j, SelfSearchConfig& config) {
    config.selfIp_           = j.value("selfIp", "");
    config.ipToPing_         = j.value("ipToPing", "");
    config.destIpAddress_    = j.value("DestIpAddress", "");
    config.defaultGateway_   = j.value("DefaultGateway", "");
    config.subnetMask_      = j.value("SubnetMask", "255.255.255.0");
    config.remoteIp_         = j.value("RemoteIp", "");
    config.remoteIpDestination_ = j.value("RemoteIpDestination", "");
    config.role_            = j.value("role", "source1");
}
